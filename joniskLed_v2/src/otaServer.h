#include "HttpsOTAUpdate.h"

static HttpsOTAStatus_t otastatus;

void startOtaServer(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attemptCount = 0;
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
      attemptCount++;
      if(attemptCount >= 10){
        ESP.restart();
      }
  }
  blinkLed(1, 250, 3);
  setLED(1, 50);

  Serial.print("Connected to ");
  Serial.println(ssid);

  // HttpsOTA.onHttpEvent(HttpEvent);
  Serial.println("Starting OTA");
  HttpsOTA.begin(url, server_certificate);

  Serial.println("Please Wait it takes some time ...");
}
