#include <Adafruit_NeoPixel.h>

#include <esp_now.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "EEPROM.h"

#include "Wavetable.h"
#include "Osc.h"
#include "pEnv.h"
#include "credentials.h"

#define NUM_WAVETABLES  8
#define NUM_ENV 8
#define NUM_OSC 8
#define FPS 60
#define CHANNEL 6

#define PIN        4
#define GATE  16
#define NUMPIXELS 60

Wavetable* wavetables[NUM_WAVETABLES];
Osc* osc[NUM_OSC];
pEnv envelopes[NUM_ENV];

unsigned char channel = 0;
unsigned char id = 0;
unsigned char colorMul[3] = {0, 0, 1};

float bitMap[NUMPIXELS];

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

char msg[256];
bool bNewMsg = false;

enum Mode {
  Raw,              // 0x00
  Commands,         // 0x01
  Test,             // 0x02
  StartOTA,         // 0x03
  HandleOTA         // 0x04
};

int frameDur = 1000 / FPS;
double lastUpdated = 0;

Mode mode = Commands;
float brightness = 1.0;
float addValue = 1.0;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(5, OUTPUT); // Small red SMD LED
  digitalWrite(5, HIGH); delay(100); digitalWrite(5, LOW);
  delay(1000);
  WiFi.softAP("LEDBEAM", "nonsense", CHANNEL, true);
  WiFi.mode(WIFI_STA);

  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress()); // This is the mac address of the Slave in AP Mode

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

  //  Serial.println("Ready");
  //  Serial.print("IP address: ");
  //  Serial.println(WiFi.localIP());

  if (!EEPROM.begin(64)) {
    Serial.println("failed to initialise EEPROM");
  }
  id = EEPROM.read(0);
  channel = EEPROM.read(1);

  pixels.begin();

  digitalWrite(5, HIGH); delay(50); digitalWrite(5, LOW);
  pinMode(GATE, OUTPUT); // PWM signal or simply use it as a gate
  digitalWrite(GATE, HIGH);

  pixels.setPixelColor(0, pixels.Color(10, 10, 10));
  pixels.show();

  initWavetables();

  for (int i = 0; i < NUM_OSC; i++) {
    osc[i] = nullptr;
  }
  memset(bitMap, 0x00, NUMPIXELS * sizeof(float));

  osc[0] = new Osc(wavetables[0]);
  osc[0]->start();
}

void loop() {
  switch (mode) {
    case Raw: {
        if (bNewMsg) {
          for (int i = 0; i < NUMPIXELS; i++) {
            pixels.setPixelColor(i, pixels.Color(msg[i] * colorMul[0], msg[i] * colorMul[1], msg[i] * colorMul[2]));
          }
          pixels.show();
          bNewMsg = false;
        }
      }
      break;
    case Commands: {
        // Parse the command
        checkCmd();
        // Update the oscillators
        for (int i = 0; i < NUM_OSC; i++) {
          if (osc[i] != nullptr) {
            if (osc[i]->bActive) {
              osc[i]->update();
              osc[i]->write(bitMap, NUMPIXELS, BLEND_NONE); // BlendMax: Check previous value in buffer, take highest
            }
          }
        }

        updateEnvelopes();
        if (millis() - lastUpdated > (frameDur)) {
          for (int i = 0; i < NUMPIXELS; i++) {
            pixels.setPixelColor(i, pixels.Color(
                                   bitMap[i] * colorMul[0] * brightness * 255,
                                   bitMap[i] * colorMul[1] * brightness * 255,
                                   bitMap[i] * colorMul[2] * brightness * 255
                                 ));
          }
          pixels.show();
          lastUpdated = millis();
        }
      }
      break;
    case Test: {
        pixels.clear(); // Set all pixel colors to 'off'
        for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
          float b = pow(sin(3.14 * ((i + (millis() / 30) % NUMPIXELS) * (1. / NUMPIXELS))), 10.);
          if (b < 0.3) {
            b == 0.3;
          };
          pixels.setPixelColor(i, pixels.Color(255 * b, 255 * b, 255 * b));
        }
        pixels.show();   // Send the updated pixel colors to the hardware.

        delay(1000 / 60); // 60 fps
      }
      break;
    case StartOTA: {
        WiFi.begin(ssid, password);
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
          Serial.println("Connection Failed! Rebooting...");
          pixels.clear();
          pixels.setPixelColor(0, pixels.Color(255, 0, 0));
          pixels.show();
          delay(1000);
          ESP.restart();
        }
        pixels.clear();
        pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        pixels.show();

        ArduinoOTA.onStart([]() {
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
        mode = HandleOTA;
      }
      break;
      case HandleOTA:{
        ArduinoOTA.handle();        
      }
      break;
  }
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  //  digitalWrite(5, HIGH); delay(50); digitalWrite(5, LOW);
  unsigned char msgType = data[0];
  if (msgType == channel) { // Receive raw pixels
    memcpy(msg, data + 1 + (NUMPIXELS * id), NUMPIXELS);
    bNewMsg = true;
  } else {
    switch (msgType) {
      case 0x03:
        id = data[1];
        writeEEPROM();
        break;
      case 0x04:
        channel = data[1];
        writeEEPROM();
        break;
      case 0x06:
        memcpy(colorMul, data + 1, 3);
        break;
      case 0x08:
        memcpy(msg, data + 1, data_len - 1); // Ignore global-type
        bNewMsg = true;
        break;
      case 0x09:
      mode = StartOTA;
        break;
      case 0x10:
        mode = (Mode)data[1];
        break;
    }
  }
}

void writeEEPROM() {
  EEPROM.write(0, id);
  EEPROM.write(1, channel);
  EEPROM.commit();
}

void initWavetables() {
  int resolution = 1024;

  wavetables[0] = new Wavetable(resolution);
  wavetables[0]->fillSine();
  for (int i = 0; i < 4; i++) {
    wavetables[i + 1] = new Wavetable(resolution);
    wavetables[i + 1]->fillSine(i + 2);
  }
  for (int i = 0; i < 3; i++) {
    wavetables[i + 5] = new Wavetable(resolution);
    wavetables[i + 5]->fillSine(pow(i + 6, 2));
  }
  //  for(int i=0; i<3; i++){
  //    wavetables[i+5] = new Wavetable(resolution);
  //    wavetables[i+5]->fillSine(1./(i+2.));
  //  }
}

void checkCmd() {
  if (bNewMsg) {
    switch (msg[0]) {
      case 0x00: { // Set freq
          if (osc[msg[1]]) {
            double f;
            memcpy(&f, msg + 2, 8);
            osc[msg[1]]->frequency = f;
          }
        }
        break;
      case 0x01: {
          if (osc[msg[1]]) {
            double phase;
            memcpy(&phase, msg + 2, 8);
            osc[msg[1]]->phase = phase;
          }
        }
        break;
      case 0x02: {
          if (osc[msg[1]]) {
            double offset;
            memcpy(&offset, msg + 2, 8);
            osc[msg[1]]->offset = offset;
          }
        }
        break;
      case 0x03: {
          if (osc[msg[1]]) {
            double range;
            memcpy(&range, msg + 2, 8);
            osc[msg[1]]->range = range;
          }
        }
        break;
      case 0x04: {
          if (osc[msg[1]] && msg[2] < NUM_WAVETABLES) {
            osc[msg[1]]->setWavetable(wavetables[msg[2]]);
          }
        }
        break;
      case 0x05: {
          pEnv* e = findIdleEnv();
          double a, s, r, b;
          memcpy(&a, msg + 1 + (8 * 0), 8);
          memcpy(&s, msg + 1 + (8 * 1), 8);
          memcpy(&r, msg + 1 + (8 * 2), 8);
          memcpy(&b, msg + 1 + (8 * 3), 8);
          if (e)
            e->trigger(a, s, r, b);
        }
        break;
      case 0x06: {
          double value;
          memcpy(&value, msg + 1, 8);
          addValue = value;
        }
        break;
    }
  }
  bNewMsg = false;
}

void updateEnvelopes() {
  float maxValue = 0;
  for (char j = 0; j < NUM_ENV; j++) {
    float envVal = envelopes[j].update();
    if (envVal > maxValue) {
      maxValue = envVal;
    }
  }

  if (addValue > maxValue) // Offsetvalue
    maxValue = addValue;

  brightness = maxValue;
}

pEnv* findIdleEnv() {
  for (char i = 0; i < NUM_ENV; i++) {
    if (envelopes[i].bActive == false) {
      return envelopes + i;
    }
  }
  return nullptr;
}
