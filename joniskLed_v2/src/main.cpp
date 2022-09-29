#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "EEPROM.h"
#include "credentials.h"

char* ssid;
char* password;
char* url; // Set from MSG

#define CHANNEL 1
// #define IS_JONISK_2022
// #define JONISK // This performs the battery measurement
// #define IS_PONTLED_2019
#define IS_PONTLED_2022_V1
// #define IS_LED_FIXBOARD

#define   PWM_10_BIT

unsigned char values[4] = {0, 0, 0, 0};
uint8_t replyAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t myAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

enum Mode {NOLAG, LAG, START_WIFI, HANDLE_OTA, SEND_BATTERY, DEEP_SLEEP, HANDLE_OTA_SERVER, START_OTA_SERVER};
Mode mode = NOLAG;
Mode modeToReturnTo = Mode::NOLAG;
unsigned char id = 0;
bool bUpdate = false;
unsigned long lastReceived = 0;
unsigned long lastChecked = 0;

void blinkLed(int channel, int delayTime, int num, int brightness=50);
void checkPins();

#include "pinMap.h"
#include "espnowFunctions.h"
#include "batteryStatus.h"

uint8_t staticIndex = 0;

unsigned short lagTime = 100;
unsigned char startValues[4];
unsigned char endValues[4];
unsigned long envEndTime = 0;
unsigned long envStartTime = 0;
bool bLagDone = false;

#include "ledFunctions.h"
#include "otaServer.h"

void writeEEPROM(){
  EEPROM.write(0, id);
  EEPROM.commit();
}

void setup() {
  delay(500);
  initPins();
  // aliveBlink(true);
  // digitalWrite(5, HIGH);
  // return;
//  pinMode(23, OUTPUT); digitalWrite(23, LOW);
  ledcAttachPin(R_PIN, 1); // assign RGB led pins to channels
  ledcAttachPin(G_PIN, 2);
  ledcAttachPin(B_PIN, 3);
  ledcAttachPin(W_PIN, 4);

  // Initialize channels
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  for(char i=0; i<4; i++){
#ifdef  PWM_10_BIT
    ledcSetup(i+1, 9000, 12); // Hz / bitdepth
#else
    ledcSetup(i+1, 12000, 8); // Hz / bitdepth
#endif
    ledcWrite(i+1, 0);// Turn all off
  }

  Serial.begin(115200);
  Serial.println("Serial opened, led pwm channels already configured");

  delay(200);
  WiFi.softAP("TIYCS", "nonsense", 1, true);
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  WiFi.softAPmacAddress(myAddr);

  initESPNow();
  Serial.println("Register callback");
  esp_now_register_recv_cb(OnDataRecv);

  if (!EEPROM.begin(64)){
    Serial.println("failed to initialise EEPROM");
  }
  id = EEPROM.read(0);

  aliveBlink(); // BuiltinLED
  initCurve();
  testLed();
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
       for(staticIndex=0; staticIndex<4; staticIndex++){
        setLED(staticIndex, values[staticIndex]);
       }
      bUpdate = false;
      }
    }
    break;
    case LAG:{
      if(millis() < envEndTime){
        float ratio = (millis() - envStartTime) / (float)lagTime; // 0.0 - 1.0
        for(staticIndex=0; staticIndex<4; staticIndex++){
         values[staticIndex] = (startValues[staticIndex] * (1-ratio)) + (endValues[staticIndex] * ratio);
         ledcWrite(staticIndex+1, values[staticIndex]);
        }
      } else{
        if(!bLagDone){
          for(staticIndex=0; staticIndex<4; staticIndex++){
            if(values[staticIndex] != endValues[staticIndex]){
              values[staticIndex] = endValues[staticIndex];
              ledcWrite(staticIndex+1, values[staticIndex]);
            }
          }
          bLagDone = true;
        }
      }
    }
    break;
    case START_WIFI:{ // Start the WiFi, and then change to HANDLE_OTA mode
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Go to normal state...");
        blinkLed(0, 50, 1);
        ESP.restart();
      }

      blinkLed(1, 250, 3);
      setLED(1, 50);

      ArduinoOTA
        .onStart([]() {
          String type;
          if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
          else // U_SPIFFS
            type = "filesystem";

          // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
          Serial.println("Start updating " + type);
        })
        .onEnd([]() {
          blinkLed(1, 500, 1, 100);
        })
        .onProgress([](unsigned int progress, unsigned int total) {
          int percentage = (progress / (total / 100));
          setLED(1, percentage / 2); // 0 - 50
        })
        .onError([](ota_error_t error) {
          Serial.printf("Error[%u]: ", error);
          blinkLed(0, 30, 3, 40);
          if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
          else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
          else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
          else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
          else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });

      ArduinoOTA.begin();

      Serial.print("Ota Ready, ");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      mode = HANDLE_OTA;
    }
    break;
    case HANDLE_OTA:{
        ArduinoOTA.handle();
        delay(10);
    }
    break;
    case START_OTA_SERVER:{
      startOtaServer();
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
      esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,1);
      esp_deep_sleep_start();
    }
  }
}

void checkPins(){
  if(millis() > lastChecked + 100){
    #ifdef  IS_JONISK_2022
    if(digitalRead(15) == LOW){
      digitalWrite(5, HIGH);
      delay(100);
      mode = START_WIFI;
    }
    if(digitalRead(14) == HIGH){
      blinkInterval[0] = 130;
      blinkInterval[1] = 300;
    } else{
      blinkInterval[0] = 30;
      blinkInterval[1] = 1500;
    }
    #endif
    lastChecked = millis();
  }
}
