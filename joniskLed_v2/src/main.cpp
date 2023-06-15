#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "EEPROM.h"
#include "credentials.h"
#include "lag.h"
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

char version[2] = {1, 1}; // A 0 isn't printed in SC, so use 1.1 as first version :)

char* ssid;
char* password;
char* url; // Set from MSG

#define CHANNEL 1
#define IS_JONISK_2022_EXTRA
#define JONISK_BATTERY_CHECK
#define   PWM_12_BIT

unsigned char values[4] = {0, 0, 0, 0};
uint8_t replyAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t myAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

enum Mode {
  NOLAG,
  LAG,
  START_WIFI,
  HANDLE_OTA,
  SEND_BATTERY,
  DEEP_SLEEP,
  HANDLE_OTA_SERVER,
  START_OTA_SERVER
};

Mode mode = NOLAG;
Mode modeToReturnTo = Mode::NOLAG;

unsigned char id = 0; // Read from EEPROM
bool bUpdate = false;
unsigned long lastReceived = 0;
unsigned long lastCheckedPins = 0;
unsigned short minutesToSleep = 0;

void blinkLed(int channel, int delayTime, int num, int brightness=50);
void checkPins();

#include "pinMap.h"
#include "espnowFunctions.h"
#include "batteryStatus.h"

uint8_t i = 0;

#include "ledFunctions.h"
#include "otaServer.h"
#include "arduinoOta.h"

void setup() {
  delay(500);
  initPins();

  ledcAttachPin(R_PIN, 1); // assign RGB led pins to channels
  ledcAttachPin(G_PIN, 2);
  ledcAttachPin(B_PIN, 3);
  ledcAttachPin(W_PIN, 4);

  for(i=0; i<4; i++){
#ifdef  PWM_12_BIT
    ledcSetup(i+1, 9000, 12); // Hz / bitdepth
#else
    ledcSetup(i+1, 12000, 8); // Hz / bitdepth
#endif
    ledcWrite(i+1, 0);// Turn all off
  }

  Serial.begin(115200);
  Serial.println("Serial opened, led pwm channels already configured");

  delay(200);
  WiFi.softAP("TIYCS", "nonsense", CHANNEL, true);
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  WiFi.softAPmacAddress(myAddr);

  initESPNow();
  Serial.println("Register callback");
  esp_now_register_recv_cb(OnDataRecv);

  if (EEPROM.begin(64)){
    id = EEPROM.read(0);
    Serial.print("My ID: "); Serial.println((int)id);
  } else{
    Serial.println("failed to initialise EEPROM, id not read. Default to 0");
  }

  aliveBlink(); // BuiltinLED
  initCurve();
  testLed();
  delay(500);
  blinkVersion();
  Serial.println("Setup done");

  sendPing(true); // "I'm alive!"
}

void loop() {
  aliveBlink();
  // return;

  // return;
  sendPing();
  checkPins();
  switch(mode){
    case NOLAG:{ // Just set the PWM-channels
      if(bUpdate){
       for(i = 0; i<4; i++){
        setLED(i, values[i]);
       }
      bUpdate = false;
      }
    }
    break;
    case LAG:{
      if(millis() < envEndTime){ // envEndTime is set in the ESPNOW-receive func
        float ratio = (millis() - envStartTime) / (float)lagTime; // 0.0 - 1.0
        for(i = 0; i<4; i++){
         values[i] = (startValues[i] * (1-ratio)) + (endValues[i] * ratio);
         setLED(i, values[i]);
        }
      } else{
        if(!bLagDone){
          for(i = 0; i<4; i++){
            if(values[i] != endValues[i]){
              values[i] = endValues[i];
              setLED(i, values[i]);
            }
          }
          bLagDone = true;
        }
      }
    }
    break;
    case START_WIFI:{
      startArduinoOta();
      mode = HANDLE_OTA;
    }
    break;
    case HANDLE_OTA:{
        ArduinoOTA.handle();
        delay(10);
    }
    break;
    case START_OTA_SERVER:{
      if(startOtaServer()){
        blinkLed(1, 250, 3);
        setLED(1, 50);
      } else{
        ESP.restart();
      }
      mode = HANDLE_OTA_SERVER;
    }
    break;
    case HANDLE_OTA_SERVER:{
      otastatus = HttpsOTA.status();
      if(otastatus == HTTPS_OTA_SUCCESS) {
          Serial.println("Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
          ESP.restart();
      } else if(otastatus == HTTPS_OTA_FAIL) {
          Serial.println("Firmware Upgrade Fail");
      }
      delay(1000);
    }
    break;
    case SEND_BATTERY:{
      sendBatteryStatus();
      WiFi.mode(WIFI_OFF);
      WiFi.softAP("TIYCS", "nonsense", 1, true);
      Serial.println("Setup ESPNOW");
      initESPNow();
      Serial.println("Register callback");
      esp_now_register_recv_cb(OnDataRecv);
      mode = modeToReturnTo;
    break;
    }
    case DEEP_SLEEP:{
      // Go to deep sleep
      blinkLed(3, 250, 2, 20);
      turnLedOff();
      if(minutesToSleep){
        // esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,1);
        esp_sleep_enable_timer_wakeup(minutesToSleep * 60 * uS_TO_S_FACTOR);
        esp_deep_sleep_start();
      }
    }
    break;
  }
}

void checkPins(){
  if(millis() > lastCheckedPins + 100){
    #ifdef  IS_JONISK_2022
    if(digitalRead(15) == LOW){
      digitalWrite(5, HIGH);
      delay(100);
      mode = START_WIFI;
    }
    if(digitalRead(14) == HIGH){ // Charging indicator
      blinkInterval[0] = 130;
      blinkInterval[1] = 300;
    } else{
      blinkInterval[0] = 30;
      blinkInterval[1] = 1500;
    }
    #endif
    lastCheckedPins = millis();
  }
}
