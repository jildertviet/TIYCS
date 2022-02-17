#include <esp_now.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "EEPROM.h"
#include "credentials.h"

#define CHANNEL 1
#define IS_JONISK_2022
#define   PWM_10_BIT

unsigned char values[4] = {0, 0, 0, 0};
uint8_t replyAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

enum Mode {NOLAG, LAG, START_WIFI, HANDLE_OTA, SEND_BATTERY};
Mode mode = NOLAG;
Mode modeToReturnTo = Mode::NOLAG;
unsigned char id = 0;
bool bUpdate = false;
unsigned long lastReceived = 0;

void blinkLed(int channel, int delayTime, int num, int brightness=50);

#include "pinMap.h"
#include "espnowFunctions.h"
#include "batteryStatus.h"

float brightnessCurve[256];
char staticIndex = 0;

unsigned short lagTime = 100;
unsigned char startValues[4];
unsigned char endValues[4];
unsigned long envEndTime = 0;
unsigned long envStartTime = 0;
bool bLagDone = false;

unsigned long lastBlinked = 0;
int blinkInterval[2] = {30, 1300};
char blinkIndex = 0;


void testLed(){
  for(char i=0; i<4; i++){
    setLED(i, 50);
    delay(500);
    turnLedOff();
  }
}

void turnLedOff(){
  for(char i=0; i<4; i++){
    ledcWrite(i+1, 0);
  }
}

void setLED(int channel, int value){ // Receives 0 - 255
  float v = brightnessCurve[value];
  if(channel == 3){
    int sumOfRGB = values[0] + values[1] + values[2];
    if(sumOfRGB >= 290){
      if(value>225){
        value = 225; // Limit (RGB255 + W225 = 4.0A)
      }
    }
  }
  values[channel] = value;
#ifdef PWM_10_BIT
  value = v * 1024;
#endif

  ledcWrite(channel + 1, value);
}

void aliveBlink(){
  if(millis() > lastBlinked + blinkInterval[blinkIndex]){
    lastBlinked = millis();
    digitalWrite(5, blinkIndex);
    blinkIndex = (blinkIndex + 1) % 2; // 0, 1, 0, etc
  }
}

void sendPing(){
  // Only send when no msg is received for x seconds
  if(millis() > lastReceived + 60000 && millis() > 10000){
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  
    initESPNow();
    esp_now_register_recv_cb(OnDataRecv);
    uint8_t broadcastAddr[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    memcpy(replyAddr, &broadcastAddr, 6);
    addPeer(replyAddr);
  
    uint8_t msg[5] = {'a','l', 'i', 'v', 'e'};
//    int v = measureBattery();
//    memcpy(msg+1, &v, 4); // Prefix is 'b'
    
    esp_err_t result = esp_now_send(replyAddr, msg, 5);
//    if (result == ESP_OK) {
//      blinkLed(1, 100, 1);
//    } else {
//      blinkLed(0, 50, 2);
//    }
    WiFi.mode(WIFI_OFF);
    WiFi.softAP("TIYCS", "nonsense", 1, true);
    Serial.println("Setup ESPNOW");
    initESPNow();
    Serial.println("Register callback");
    esp_now_register_recv_cb(OnDataRecv);
    lastReceived = millis();
  }
}

void blinkLed(int channel, int delayTime, int num=1, int brightness){
  for(int i=0; i<num; i++){
      setLED(channel, brightness);
      delay(delayTime);
      setLED(channel, 0);
      delay(delayTime);
  }
  setLED(channel, 0);
}

void writeEEPROM(){
  EEPROM.write(0, id);
  EEPROM.commit();
}

void initCurve(){
  for(int i=0; i<256; i++){
    if(i < 30){
      float v = i / 30.0;
      v = pow(v, 0.5);
      v *= 30.0;
      brightnessCurve[i] = pow((v / 256.), 2.0);
    } else{
      brightnessCurve[i] = pow((i / 256.), 2.0);
    }
  }
}

void setup() { 
//  delay(1000);
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
    ledcSetup(i+1, 9000, 10); // Hz / bitdepth
#else
    ledcSetup(i+1, 12000, 8); // Hz / bitdepth
#endif
    ledcWrite(i+1, 0);// Turn all off
  }

  Serial.begin(115200);
  Serial.println("Serial opened, led pwm channels already configured");
  
  delay(200);
//  WiFi.mode(WIFI_AP);
  WiFi.softAP("TIYCS", "nonsense", 1, true);
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());

  initESPNow(); 
  Serial.println("Register callback");
  esp_now_register_recv_cb(OnDataRecv);
  
  if (!EEPROM.begin(64)){
    Serial.println("failed to initialise EEPROM");
  }
  id = EEPROM.read(0);

  pinMode(34, INPUT);
  pinMode(14, INPUT);
  pinMode(4, OUTPUT);
  
  delay(50);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH); delay(50); digitalWrite(5, LOW);
  delay(50);
  Serial.println("init curve");
  initCurve();
  delay(50);
  Serial.println("test led");
  testLed();
//  ledcWrite(1, 1024*0.25);
  Serial.println("Setup done");
}

void loop() { 
  aliveBlink();
  sendPing();
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
      esp_now_register_recv_cb(OnDataRecv);
      mode = modeToReturnTo;
    break;
    }
  }
}
