#include <esp_now.h>
#include <WiFi.h>

esp_now_peer_info_t slave;

unsigned char serialBuffer[256];
unsigned char writePos = 0;
unsigned char addr[6];

unsigned char x;
unsigned char lengtOfMsg;
unsigned char startByte;

//#define J_LOG true

#define CHANNEL 11
#define NUM 5
const uint8_t mac[NUM][6] = {
//  {0x7C,0xDF,0xA1,0x03,0x9A,0x3F}
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
  {0x7C,0xDF,0xA1,0x03,0x9A,0x47},
  {0x7C,0xDF,0xA1,0x03,0x9A,0x51},
  {0x7C,0xDF,0xA1,0x03,0x9A,0x55},
  {0x7C,0xDF,0xA1,0x03,0x9A,0x3F}
};

void initESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

bool manageSlave() {
  if (slave.channel == CHANNEL) {
    Serial.print("Slave Status: ");
    const esp_now_peer_info_t *peer = &slave;
    const uint8_t *peer_addr = slave.peer_addr;
    bool exists = esp_now_is_peer_exist(peer_addr);
    if ( exists) {
      Serial.println("Already Paired");
      return true;
    } else {
      // Slave not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(peer);
      if (addStatus == ESP_OK) {
        // Pair success
        Serial.println("Pair success");
        return true;
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        Serial.println("ESPNOW Not Init");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        Serial.println("Peer list full");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("Out of memory");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        Serial.println("Peer Exists");
        return true;
      } else {
        Serial.println("Not sure what happened");
        return false;
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
    return false;
  }
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
#if J_LOG
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
#endif
}

void setup() {
  memset(serialBuffer, 256, 0);
  Serial.begin(230400);
  
  WiFi.mode(WIFI_STA);
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());

  initESPNow();
  esp_now_register_send_cb(onDataSent);

  slave.channel = CHANNEL;
  slave.encrypt = 0;
  for(char i=0; i<NUM; i++){
    memcpy(slave.peer_addr, mac[i], 6);      
    manageSlave();
  }
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  digitalWrite(5, LOW); delay(50); digitalWrite(5, HIGH);
}

void loop() {
  while(Serial.available()){
    x = Serial.read();
//    Serial.println((int)x);
    serialBuffer[writePos] = x;
    if(serialBuffer[writePos] == 'd' && writePos > 9){
      if(serialBuffer[writePos-1] == 'n' && serialBuffer[writePos-2] == 'e'){
        int len = writePos-2-6;
//        for(int i=0; i<len; i++){
//          Serial.print((char)serialBuffer[i]);
//        }
//        Serial.println();
        memcpy(addr, serialBuffer, 6);
        for(int i=0; i<6; i++){
          Serial.print((int)addr[i]); Serial.print(" ");
        }
        Serial.println();
        for(int i=0; i<len; i++){
          Serial.print((int)serialBuffer[i+6]); Serial.print(" ");
        }
        Serial.println();
        esp_now_send(addr, serialBuffer+6, len);
//        digitalWrite(5, LOW); delay(50); digitalWrite(5, HIGH);
        writePos = 0;
        return;
      }
    } 
    writePos++;
  }
}
