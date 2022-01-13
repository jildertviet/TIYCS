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

enum Mode {NOLAG, LAG, START_WIFI, HANDLE_OTA, SEND_BATTERY};
Mode mode = NOLAG;
unsigned short lagTime = 100;
unsigned char startValues[4];
unsigned char endValues[4];
unsigned long envEndTime = 0;
unsigned long envStartTime = 0;
bool bLagDone = false;
Mode modeToReturnTo = Mode::NOLAG;
unsigned char id = 0;

void setup() { 
  pinMode(23, OUTPUT); digitalWrite(23, LOW);
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
  testLed();
  Serial.println("Setup done");
}

void blinkLed(int channel, int delayTime, int num=1){
  for(int i=0; i<num; i++){
      setLED(channel, 255);
      delay(delayTime);
      setLED(channel, 0);
      delay(delayTime);
  }
  setLED(channel, 0);
}

void loop() { 
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
    case SEND_BATTERY:{ // Test ... 
      uint8_t addr[6] = {0x30,0xae,0xa4,0x84,0x1c,0xbc}; // Can't reply, since it isn't registered?
      esp_now_peer_info_t slave;

      slave.channel = CHANNEL;
      slave.encrypt = 0;
      memcpy(slave.peer_addr, addr, 6);      
      if (slave.channel == CHANNEL) {
    Serial.print("Slave Status: ");
    const esp_now_peer_info_t *peer = &slave;
    const uint8_t *peer_addr = slave.peer_addr;
    bool exists = esp_now_is_peer_exist(peer_addr);
    if ( exists) {
      Serial.println("Already Paired");
    } else {
      // Slave not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(peer);
      if (addStatus == ESP_OK) {
        // Pair success
        Serial.println("Pair success");
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        Serial.println("ESPNOW Not Init");
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        Serial.println("Peer list full");
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("Out of memory");
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        Serial.println("Peer Exists");
      } else {
        Serial.println("Not sure what happened");
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
  }
      
      uint8_t msg[4] = {'t', 'e', 's', 't'};
//      float v = measureBattery();
//      memcpy(&msg, &v, 4);
      
      esp_err_t result = esp_now_send(addr, msg, 4);
      if (result == ESP_OK) {
        blinkLed(2, 100, 1);
      } else {
        blinkLed(0, 50, 2);
      }
      mode = modeToReturnTo;
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
//  digitalWrite(5, HIGH); delay(50); digitalWrite(5, LOW);

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
      mode = Mode::START_WIFI; 
      break;
    case 0x08: // Reply with battery voltage
      mode = Mode::SEND_BATTERY;
      break;
  }
}

void writeEEPROM(){
  EEPROM.write(0, id);
  EEPROM.commit();
}

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



void setLED(int channel, int value){
#ifdef PWM_10_BIT
  float v = value / 255.;
  v = pow(v, 2.0);
  ledcWrite(channel + 1, v * 1024);
#else
  ledcWrite(channel + 1, v);
#endif
}

float measureBattery(){
  return analogRead(34); // 0 <> 4095
}
