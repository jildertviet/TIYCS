
int measureBattery(){
  int numSamples = 5;
  int total = 0;
  for(int i=0; i<numSamples; i++){
    total += analogRead(34);
    delay(20);
  }
  total /= numSamples;
  return total; // 0 <> 4095
}

void sendBatteryStatus(){
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());

  initESPNow();

  addPeer(replyAddr);

  uint8_t msg[5] = {'b','t', 'e', 's', 't'};
  int v = measureBattery();
  memcpy(msg+1, &v, 4); // Prefix is 'b'

  esp_err_t result = esp_now_send(replyAddr, msg, 5);
  if (result == ESP_OK) {
    blinkLed(2, 100, 1);
  } else {
    blinkLed(0, 50, 2);
  }

  delay(200);
}
