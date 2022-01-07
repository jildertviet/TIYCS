#include <esp_now.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "EEPROM.h"
#include "credentials.h"

#define CHANNEL 1

#define IS_JONISK_2022

#ifdef  IS_JONISK
  #define R_PIN 13
  #define G_PIN 12
  #define B_PIN 22
  #define W_PIN 23
#endif

#ifdef IS_LED // Used for 
  #define R_PIN 16
  #define G_PIN 33
  #define B_PIN 17
  #define W_PIN 32
#endif
#ifdef IS_JONISK_NEW
  #define R_PIN 46
  #define G_PIN 45
  #define B_PIN 41
  #define W_PIN 42
#endif
#ifdef IS_JONISK_2022
  #define R_PIN 16
  #define G_PIN 17
  #define B_PIN 18
  #define W_PIN 19
#endif

unsigned char values[4] = {0, 0, 0, 0};
bool bUpdate = false;
char staticIndex = 0;

enum Mode {NOLAG, LAG, START_WIFI, HANDLE_OTA};
Mode mode = NOLAG;
unsigned short lagTime = 100;
unsigned char startValues[4];
unsigned char endValues[4];
unsigned long envEndTime = 0;
unsigned long envStartTime = 0;
bool bLagDone = false;

unsigned char id = 0;

void setup() { 
  ledcAttachPin(R_PIN, 1); // assign RGB led pins to channels
  ledcAttachPin(G_PIN, 2);
  ledcAttachPin(B_PIN, 3);
  ledcAttachPin(W_PIN, 4);
  
  // Initialize channels 
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  for(char i=0; i<4; i++){
    ledcSetup(i+1, 9000, 10); // Hz / bitdepth
    ledcWrite(i+1, 0);// Turn all off
  }

  Serial.begin(115200);
  Serial.println("Serial opened, led pwm channels already configured");
  delay(200);
//  WiFi.mode(WIFI_AP);
  WiFi.softAP("TIYCS", "nonsense", 1, true);
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());

  Serial.println("Setup ESPNOW");
  InitESPNow(); // Init ESPNow with a fallback logic
  Serial.println("Register callback");
  esp_now_register_recv_cb(OnDataRecv);

  if (!EEPROM.begin(64)){
    Serial.println("failed to initialise EEPROM");
  }
  id = EEPROM.read(0);

  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH); delay(50); digitalWrite(5, LOW);
  Serial.println("Setup done");
}

void loop() { 
  switch(mode){
    case NOLAG:{ // Just set the PWM-channels 
      if(bUpdate){
       for(staticIndex=0; staticIndex<4; staticIndex++) // 1, 2, 3, 4
         ledcWrite(staticIndex+1, values[staticIndex]); // 0, 1, 2, 3
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
        ledcWrite(0+1, 255);
        delay(50);
        ledcWrite(0+1, 0);
        delay(50);
        ledcWrite(0+1, 255);
        delay(50);
        ESP.restart();
      }
      
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
          Serial.println("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
          Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
          Serial.printf("Error[%u]: ", error);
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
    }
    break;
  }

}

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    ESP.restart();     // or Simply Restart
  }
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char msgType = data[0];
//    digitalWrite(5, HIGH); delay(50); digitalWrite(5, LOW);

//  for(int i=0; i<data_len; i++){
//    Serial.print((int)data[i]); Serial.print(" ");
//  }
//  Serial.println();
  switch(msgType){
    case 0x05: 
      memcpy(values, data + 1 + (id*4), 4); 
      mode = NOLAG; 
      bUpdate = true;
      break;
//    case 1: 
//      mode = LAG; 
//      if(data_len > 6) // [R, G, B, W, mode, lagTime, lagTime
//        memcpy(&lagTime, data+5, 2); // Copy the last two char's, and write to unsigned short. 
//      memcpy(endValues, data, 4);
//      memcpy(startValues, values, 4);
//      envStartTime = millis();
//      envEndTime = millis() + lagTime;
//      bLagDone = false;
//    break;
    case 0x03:
      id = data[1];
      writeEEPROM();
      break;
    case 0x07: 
      mode = START_WIFI; 
      break;
  }
}

void writeEEPROM(){
  EEPROM.write(0, id);
  EEPROM.commit();
}
