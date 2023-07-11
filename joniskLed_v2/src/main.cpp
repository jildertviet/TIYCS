#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "EEPROM.h"
#include "credentials.h"
#include "lag.h"
#include "global.h"

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

char version[2] = {1, 1}; // A 0 isn't printed in SC, so use 1.1 as first version :)

#define CHANNEL 1
#define IS_JONISK_2022_EXTRA
#define JONISK_BATTERY_CHECK
#define   PWM_12_BIT

unsigned char values[4] = {0, 0, 0, 0};

#include "modes.h"
Mode mode = NOLAG;
Mode modeToReturnTo = Mode::NOLAG;

unsigned long lastCheckedPins = 0;

void checkPins();

#include "pinMap.h"
#include "otaServer.h"
#include "espnowFunctions.h"
#include "espnowFunctionsCustom.h"

#include "batteryStatus.h"

uint8_t i = 0;

#include "ledFunctions.h"
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
  handleOtaServer();
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
    case SEND_BATTERY:{
      sendBatteryStatus();
      WiFi.mode(WIFI_OFF);
      WiFi.softAP("TIYCS", "nonsense", 1, true);
      Serial.println("Setup ESPNOW");
      initESPNow();
      Serial.println("Register callback");
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
