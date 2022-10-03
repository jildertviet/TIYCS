#include "HttpsOTAUpdate.h"
#include "credentials.h"

static HttpsOTAStatus_t otastatus;

void HttpEvent(HttpEvent_t *event)
{
    switch(event->event_id) {
        case HTTP_EVENT_ERROR:
            Serial.println("Http Event Error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            Serial.println("Http Event On Connected");
            break;
        case HTTP_EVENT_HEADER_SENT:
            Serial.println("Http Event Header Sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key, event->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            break;
        case HTTP_EVENT_ON_FINISH:
            Serial.println("Http Event On Finish");
            break;
        case HTTP_EVENT_DISCONNECTED:
            Serial.println("Http Event Disconnected");
            break;
    }
}

bool checkOtaServer(){
  delay(1000);
  otastatus = HttpsOTA.status();
  if(otastatus == HTTPS_OTA_SUCCESS) {
      Serial.println("Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
      ESP.restart();
      return true;
  } else if(otastatus == HTTPS_OTA_FAIL) {
      Serial.println("Firmware Upgrade Fail");
      return false;
  }

}

bool startOtaServer(){
  WiFi.mode(WIFI_STA);
  static const char* ssidTemp = "jildert-Framework";
  static const char* passwordTemp = "Xar4wrxi";
  static const char *urlTemp = "https://10.42.0.1/firmware.bin"; //state url of your firmware image

  WiFi.begin(ssidTemp, passwordTemp);

  int attemptCount = 0;
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
      attemptCount++;
      if(attemptCount >= 10){
        return false;
      }
  }

  Serial.print("Connected to ");
  Serial.println(ssidTemp);

  HttpsOTA.onHttpEvent(HttpEvent);
  Serial.println("Starting OTA");
  HttpsOTA.begin(urlTemp, server_certificate);

  Serial.println("Please Wait it takes some time ...");
  return true;
}
