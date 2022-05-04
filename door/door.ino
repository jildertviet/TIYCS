#include <AccelStepper.h>
#include "FastLED.h"
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1
#define NUM_LEDS 86

AccelStepper stepper(AccelStepper::HALF4WIRE, 3, 2, 5, 4); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

#define DATA_PIN_A 16
#define DATA_PIN_B 17

CRGB leds[NUM_LEDS];
float range = 2000;
unsigned long startTimeFadeIn = 0;
unsigned long startTimeFadeOut = 0;
unsigned int fadeTime = 9000;

void setup(){  
  FastLED.addLeds<WS2812B, DATA_PIN_A, GRB>(leds, NUM_LEDS);  
  FastLED.addLeds<WS2812B, DATA_PIN_B, GRB>(leds, NUM_LEDS);  

//  stepper.setMaxSpeed(250);
//  stepper.setAcceleration(150);
//  stepper.moveTo(-1500);

  WiFi.softAP("TIYCS", "nonsense", 1, true);
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());

  initESPNow();
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(12, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);

  delay(2000);
  leds[0] = CRGB::Green;
  FastLED.show();
  delay(500);
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void loop(){ 
//  float ledRatio = stepper.currentPosition() / range;
//  fillLed(ledRatio);
//  blinkTest();
  if(digitalRead(12) == LOW){
//    stepper.moveTo(stepper.currentPosition() + 1500);
//    startTimeFadeOut = millis();  
    startTimeFadeOut = millis();
  }
  if(digitalRead(33) == LOW){
//    stepper.moveTo(stepper.currentPosition() - 1500);
//    startTimeFadeOut = millis();  
    startTimeFadeOut = millis();
  }
  if(digitalRead(27) == LOW){
    startTimeFadeIn = millis();
  }

  if(millis() > 10000){
    if(startTimeFadeOut + fadeTime > millis()){
      float ratio = (millis() - startTimeFadeOut) / (float)fadeTime;
      ratio = 1 - ratio;
      for(int i=0; i<NUM_LEDS; i++){
        if(i <   (ratio * NUM_LEDS)){
          leds[i] = CRGB(255 * ratio, 0, 255 * ratio);
        } else{
          leds[i] = CRGB::Black;
        }
      }
      FastLED.show();
    } else{
      if(startTimeFadeIn + fadeTime > millis()){
        float ratio = (millis() - startTimeFadeIn) / (float)fadeTime;
        for(int i=0; i<NUM_LEDS; i++){
        if(i < ratio * NUM_LEDS){
          leds[i] = CRGB(255 * ratio, 0, 255 * ratio);
        } else{
          leds[i] = CRGB::Black;
        }
        }
        FastLED.show();
      }
    }
  }
//  stepper.run();
}

void fillLed(float r){
  for(int i=0; i<NUM_LEDS; i++){
    if(NUM_LEDS / (float)i >= r){
      leds[i] = CRGB::Black;
    } else{
      leds[i] = CRGB(10, 10, 10);
    }
  }
}

void blinkTest(){
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
}

void initESPNow() {
  Serial.println("Setup ESPNOW");
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char msgType = data[0];
  switch(msgType){
    case 0x05:
      break;
    case 0x09:
    break;
    case 0x03:
      break;
    case 0x07:
      break;
    case 0x08:
      break;
    case 0x10:
      break;
    case 0x11:
      break;
    case 0x12:
    break;
    case 0x13:
    break;
    case 0x14:
    break;
    case 0x22:
    // Set door values
    stepper.moveTo(range * (data[1]/255.));
    break;
  }
}
