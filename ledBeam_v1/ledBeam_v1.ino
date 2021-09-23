#include <Adafruit_NeoPixel.h>

#include <esp_now.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "EEPROM.h"
#include "USB.h"

USBCDC USBSerial;

//const char* ssid = "Hertenkamp";
//const char* password = "Priokstraat1";

#define CHANNEL 11 // Not used

unsigned char channel = 0;
unsigned char id = 0;
unsigned char colorMul[3] = {1,1,1};

#define PIN        18
#define NUMPIXELS 57

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

char msg[256];
bool bPrintAddr = false;
bool bNewMsg = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  USBSerial.begin();  
  USBSerial.write("Booting");
  
  pinMode(5, OUTPUT); // Small red SMD LED
  digitalWrite(5, HIGH); delay(100); digitalWrite(5, LOW);
  delay(1000);
  WiFi.softAP("LEDBEAM", "nonsense", 6, true);

  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress()); // This is the mac address of the Slave in AP Mode
//  WiFi.begin(ssid, password);
//  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//    Serial.println("Connection Failed! Rebooting...");
//    delay(5000);
//    ESP.restart();
//  }

  Serial.println("Setup ESPNOW");
  WiFi.disconnect();  
  
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    ESP.restart();     // or Simply Restart
  }
  Serial.println("Register callback");
  esp_now_register_recv_cb(OnDataRecv);
 
  
//  ArduinoOTA
//    .onStart([]() {
//      String type;
//      if (ArduinoOTA.getCommand() == U_FLASH)
//        type = "sketch";
//      else // U_SPIFFS
//        type = "filesystem";
//
//      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
//      Serial.println("Start updating " + type);
//    })
//    .onEnd([]() {
//      Serial.println("\nEnd");
//    })
//    .onProgress([](unsigned int progress, unsigned int total) {
//      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
//    })
//    .onError([](ota_error_t error) {
//      Serial.printf("Error[%u]: ", error);
//      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
//      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
//      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
//      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
//      else if (error == OTA_END_ERROR) Serial.println("End Failed");
//    });
//
//  ArduinoOTA.begin();

//  Serial.println("Ready");
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());

  if (!EEPROM.begin(64)){
    Serial.println("failed to initialise EEPROM");
  }
  id = EEPROM.read(0);
  channel = EEPROM.read(1);
  
  pixels.begin();

  digitalWrite(5, HIGH); delay(50); digitalWrite(5, LOW);
  pinMode(21, OUTPUT); // PWM signal or simply use it as a gate
  digitalWrite(21, HIGH); 

  pixels.setPixelColor(0, pixels.Color(10, 10, 10));
  pixels.show();
}

void loop() {
//  ArduinoOTA.handle();
  if(bNewMsg){
    for(int i=0; i<NUMPIXELS; i++){
      pixels.setPixelColor(i, pixels.Color(msg[i] * colorMul[0], msg[i] * colorMul[1], msg[i] * colorMul[2]));
    }
    pixels.show();
    bNewMsg = false;
  }

  if(bPrintAddr == false){
    if(millis() > 5000){
      bPrintAddr = true;
      USBSerial.write(WiFi.softAPmacAddress().c_str());
    }
  }

    pixels.clear(); // Set all pixel colors to 'off'
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    float b = pow(sin(3.14*((i+(millis()/30)%NUMPIXELS)*(1./NUMPIXELS))), 10.);
    if(b<0.3){b==0.3;};
    pixels.setPixelColor(i, pixels.Color(255* b, 255 * b, 255 * b));
  }
    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(1000/60); // 60 fps
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
//  digitalWrite(5, HIGH); delay(50); digitalWrite(5, LOW);
  unsigned char msgType = data[0];
  if(msgType == channel){
    memcpy(msg, data + 1 + (NUMPIXELS*id), NUMPIXELS); 
    bNewMsg = true;
  } else if (msgType == 0x03){ 
    id = data[1];
    writeEEPROM();
  } else if(msgType == 0x04){
    channel = data[1];
    writeEEPROM();
  } else if(msgType == 0x06){
    memcpy(colorMul, data+1, 3);
  }
}

void writeEEPROM(){
  EEPROM.write(0, id);
  EEPROM.write(1, channel);
  EEPROM.commit();
}
