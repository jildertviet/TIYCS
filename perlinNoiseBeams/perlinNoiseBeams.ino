#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>
#include <ArduinoOSCWiFi.h>

#define CHANNEL 6 
#define PORT  6235

#include "espnowFunctions.h"
#include "credentials.h"

#define NUM 1
const uint8_t mac[NUM][6] = {
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
};

const uint8_t numBeams = 8;
const uint8_t pixelsPerBeam = 60;
float beamSpacing = 3;

static uint16_t xPos;
static uint16_t yPos;
static uint16_t z;

uint16_t speed = 3;
uint16_t scale = 6;
uint8_t noise[8][60];

void setup() {
  Serial.begin(115200);
  xPos = random16();
  yPos = random16();
  z = random16();
  
  WiFi.softAP("TIYCS", "nonsense", CHANNEL, true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Go to normal state...");
//    ESP.restart();
  }
  
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());

  initESPNow();
  slave.channel = CHANNEL;
  slave.encrypt = 0;
  for(char i=0; i<NUM; i++){
    memcpy(slave.peer_addr, mac[i], 6);      
    manageSlave();
  }

  unsigned char msg[4] = {0x10,0x00, 0x00, 0x00};
  esp_now_send(mac[0], msg, 4);
  delay(5);
  unsigned char msg2[4] = {0x06,0x01, 0x01, 0x01};
  esp_now_send(mac[0], msg2, 4);

  OscWiFi.subscribe(PORT, "/setNoiseValues", beamSpacing, scale, speed);
}

void fillnoise8() {
  for(int x=0; x<numBeams; x++) {
    int xOffset = scale * (x * beamSpacing);
    for(int y = 0; y< pixelsPerBeam; y++) {
      int yOffset = scale * y;
      noise[x][y] = pow(inoise8(xPos + xOffset, yPos + yOffset,z) / 255., 2.5) * 255;
//      noise[x][y] = 20;
    }
  }
  z += speed;
}

void loop() {
  fillnoise8();

  for(int h=0; h<2; h++){
    int dataLen = pixelsPerBeam * 4 + 1;
    uint8_t data[dataLen];
    data[0] = h + 1; // Channels 0x01 and 0x02
    for(int i=0; i<4; i++){
        for(int j=0; j<pixelsPerBeam; j++){
            int index = (i*pixelsPerBeam) + j;
            data[index + 1] = noise[i+(h*4)][j];
        } 
    }
    esp_now_send(mac[0], data, dataLen);
    delay(5);
//    for(int i=0; i<dataLen; i++){
//      Serial.print((int)data[i]);
//    }
//    Serial.println();
  } 
//  delay(1000/30);
}
