void startArduinoOta(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Go to normal state...");
    blinkLed(0, 50, 1);
    ESP.restart();
  }

  blinkLed(1, 250, 3);
  setLED(1, 50); // Green: connected to WiFi

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
      blinkLed(1, 500, 1, 100);
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      int percentage = (progress / (total / 100));
      setLED(1, percentage / 2); // 0 - 50
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      blinkLed(0, 30, 3, 40);
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
}
