float brightnessCurve[256];

unsigned long lastBlinked = 0;
int blinkInterval[2] = {30, 1500};
uint8_t blinkIndex = 0;

void initCurve(){
  for(int i=0; i<256; i++){
    if(i < 30){
      float v = i / 30.0;
      v = pow(v, 0.5);
      v *= 30.0;
      brightnessCurve[i] = pow((v / 256.), 2.0);
    } else{
      brightnessCurve[i] = pow((i / 256.), 2.0);
    }
  }
}

void setLED(int channel, int value){ // Receives 0 - 255
  float v = brightnessCurve[value];
  if(channel == 3){
    int sumOfRGB = values[0] + values[1] + values[2];
    if(sumOfRGB >= 290){
      if(value>225){
        value = 225; // Limit (RGB255 + W225 = 4.0A)
      }
    }
  }
  values[channel] = value;
#ifdef PWM_10_BIT
  value = v * 1024;
#endif

  ledcWrite(channel + 1, value);
}


void turnLedOff(){ // 12V PWM
  for(char i=0; i<4; i++){
    ledcWrite(i+1, 0);
  }
}

void testLed(){ // 12 PWM
  for(char i=0; i<4; i++){
    setLED(i, 50);
    delay(500);
    turnLedOff();
  }
}

void aliveBlink(){ // GPIO_5
  if(millis() > lastBlinked + blinkInterval[blinkIndex]){
    lastBlinked = millis();
    digitalWrite(5, blinkIndex);
    blinkIndex = (blinkIndex + 1) % 2; // 0, 1, 0, etc
  }
}

void sendPing(){
  if(mode ==HANDLE_OTA)
    return;

  // Only send when no msg is received for x seconds
  if(millis() > lastReceived + 60000 && millis() > 10000){
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());

    initESPNow();
    esp_now_register_recv_cb(OnDataRecv);
    uint8_t broadcastAddr[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    memcpy(replyAddr, &broadcastAddr, 6);
    addPeer(replyAddr);

    uint8_t msg[5] = {'a','l', 'i', 'v', 'e'};
    int v = measureBattery();
    memcpy(msg+1, &v, 4); // Prefix is 'a'

    // esp_err_t result = 
    esp_now_send(replyAddr, msg, 5);
//    if (result == ESP_OK) {
//      blinkLed(1, 100, 1);
//    } else {
//      blinkLed(0, 50, 2);
//    }
    WiFi.mode(WIFI_OFF);
    WiFi.softAP("TIYCS", "nonsense", 1, true);
    Serial.println("Setup ESPNOW");
    initESPNow();
    Serial.println("Register callback");
    esp_now_register_recv_cb(OnDataRecv);
    lastReceived = millis();
  }
}

void blinkLed(int channel, int delayTime, int num=1, int brightness){ // 12V PWM
  for(int i=0; i<num; i++){
      setLED(channel, brightness);
      delay(delayTime);
      setLED(channel, 0);
      delay(delayTime);
  }
  setLED(channel, 0);
}
