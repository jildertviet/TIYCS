#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>
#include "espnowFunctions.h"

#define NUM 1
const uint8_t mac[NUM][6] = {
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
};

const uint8_t numBeams = 8;
const uint8_t pixelsPerBeam = 60;
float beamSpacing = 10;

static uint16_t x;
static uint16_t y;
static uint16_t z;

uint16_t speed = 10;
uint16_t scale = 311;
uint8_t noise[8][60];

void setup() {
  Serial.begin(115200);
  x = random16();
  y = random16();
  z = random16();
  
  WiFi.softAP("TIYCS", "nonsense", 6, true);
  WiFi.mode(WIFI_STA);
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());

  initESPNow();
  slave.channel = 6;
  slave.encrypt = 0;
  for(char i=0; i<NUM; i++){
    memcpy(slave.peer_addr, mac[i], 6);      
    manageSlave();
  }

  unsigned char msg[2] = {0x10,0x00};
  esp_now_send(mac[0], msg, 2);
}

void fillnoise8() {
  for(int x=0; x<numBeams; x++) {
    int xOffset = scale * x;
    for(int j = 0; j < pixelsPerBeam; j++) {
      int yOffset = scale * y * beamSpacing;
      noise[x][y] = inoise8(x + xOffset,y + yOffset,z);
    }
  }
  z += speed;
}

void loop() {
  fillnoise8();

  for(int h=0; h<2; h++){
    uint8_t data[pixelsPerBeam * 4 + 1];
    data[0] = h + 1; // Channels 0x01 and 0x02
    for(int i=0; i<4; i++){
        for(int j=0; j<pixelsPerBeam; j++){
            int index = (i*pixelsPerBeam) + j;
            data[index + 1] = noise[i+(h*4)][j] * 0.9 + 5;
        } 
    }
    esp_now_send(mac[0], data, sizeof(data));
  } 
  delay(1000/30);
}
