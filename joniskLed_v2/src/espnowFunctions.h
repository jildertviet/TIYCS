#include <ArduinoJson.h>

esp_now_peer_info_t slave;

void writeEEPROM();

bool checkAddressed(const uint8_t* data){
  bool bAddressed = true;
  for(int i=0; i<6; i++){ // Check if this msg is for this ESP32
    if(myAddr[i] != data[i+1]){
      bAddressed = false;
    }
  }
  return bAddressed;
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char msgType = data[0];
  if(msgType != 'a'){
    lastReceived = millis();
  }
//  digitalWrite(5, HIGH); delay(50); digitalWrite(5, LOW);

 // for(int i=0; i<data_len; i++){
   // Serial.print((int)data[i]); Serial.print(" ");
 // }
//  Serial.println();
  switch(msgType){
    case 0x05:
      memcpy(values, data + 1 + (id*4), 4);
      mode = NOLAG;
      bUpdate = true;
      break;
    case 0x09:
//      mode = LAG;
//      if(data_len > 6) // [R, G, B, W, mode, lagTime, lagTime
//        memcpy(&lagTime, data+5, 2); // Copy the last two char's, and write to unsigned short.
//      memcpy(endValues, data, 4);
//      memcpy(startValues, values, 4);
//      envStartTime = millis();
//      envEndTime = millis() + lagTime;
//      bLagDone = false;
    break;
    case 0x03:
    Serial.println("Set ID");
      id = data[1];
      writeEEPROM();
      Serial.println((int)id);
      break;
    case 0x07:{
      if(checkAddressed(data)){
        mode = Mode::START_WIFI;
        memcpy(&replyAddr, mac_addr, 6);
        int seperatorIndex = -1;
        for(int i=1+6; i<data_len; i++){
          if(data[i] == ';'){
            seperatorIndex = i;
            Serial.print("sepIndex: "); Serial.println(seperatorIndex);
            break;
          }
        } // 0x07 a b c ; d d
        if(seperatorIndex>0){
          ssid = new char[seperatorIndex-1+1-6];
          password = new char[data_len - seperatorIndex + 1];
          memset(ssid, 0x00, seperatorIndex-1+1-6);
          memset(password, 0x00, data_len - seperatorIndex + 1);
          memcpy(ssid, data+1+6, seperatorIndex-1-6);
          memcpy(password, data+seperatorIndex+1, data_len - seperatorIndex - 1);
          Serial.println(ssid);
          Serial.println(password);
        }
      }
    }
      break;
    case 0x08: // Reply with battery voltage
      if(checkAddressed(data)){
        mode = Mode::SEND_BATTERY;
      }
      break;
    case 0x10:
      if(checkAddressed(data)){
        mode = Mode::DEEP_SLEEP;
      }
      break;

    case 0x11:{ // {0x11, a, d, d, r, e, s, 0x05} : set ESP32 with address 'addres' to ID 0x05
      if(checkAddressed(data)){
        uint8_t newMsg[data_len - 6];
        newMsg[0] == 0x03;
        memcpy(newMsg+1, data + 7, data_len - 6);
        OnDataRecv(mac_addr, newMsg, data_len-6); // Recursively call this function to set the ID
      }
    }
      break;
    // case 0x12:{
    //   if(checkAddressed(data)){
    //     uint8_t newMsg[data_len - 6]; // OTA
    //     newMsg[0] == 0x07;
    //     memcpy(newMsg+1, data + 7, data_len - 6);
    //     OnDataRecv(mac_addr, newMsg, data_len-6);
    //   }
    // }
    // break;
    case 0x13:{
      if(checkAddressed(data)){
        uint8_t newMsg[2] = {0x10, 0x00}; // Deep sleep
        OnDataRecv(mac_addr, newMsg, 2);
      }
    }
    break;
    case 0x14:{
      if(checkAddressed(data)){
        uint8_t newMsg[2] = {0x08, 0x00}; // Battery
        OnDataRecv(mac_addr, newMsg, 2);
      }
    }
    break;
    case 0x15:{
      Serial.print("0x15");
      if(checkAddressed(data)){
        Serial.println(" addressed");
        DynamicJsonDocument doc(200); // {"ssid":"xxx"}
        char* json = new char[data_len-7];
        memcpy(json, data+1+6, data_len - 7); // ID, mac[6], JSONMSG
        DeserializationError error = deserializeJson(doc, json);
        String ssid_ = doc["ssid"];
        ssid = new char[ssid_.length()+1];
        ssid_.toCharArray(ssid, ssid_.length()+1);
        String password_ = doc["password"];
        password = new char[password_.length()+1];
        password_.toCharArray(password, password_.length()+1);
        String url_ = doc["url"];
        url = new char[url_.length() + 1];
        url_.toCharArray(url, url_.length() + 1);
        // memcpy(url, &url_, url_.length());
        Serial.println(ssid);
        Serial.println(password);
        Serial.println(url);
        mode = START_OTA_SERVER;
      }
    }
    break;
  }
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

void addPeer(uint8_t* addr){
  slave.channel = CHANNEL;
  slave.encrypt = 0;
  memcpy(slave.peer_addr, addr, 6);
  if (slave.channel == CHANNEL) {
    Serial.print("Slave Status: ");
    const esp_now_peer_info_t *peer = &slave;
    const uint8_t *peer_addr = slave.peer_addr;
    bool exists = esp_now_is_peer_exist(peer_addr);
    if ( exists) {
      Serial.println("Already Paired");
    } else {
      // Slave not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(peer);
      if (addStatus == ESP_OK) {
        // Pair success
        Serial.println("Pair success");
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        Serial.println("ESPNOW Not Init");
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        Serial.println("Peer list full");
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("Out of memory");
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        Serial.println("Peer Exists");
      } else {
        Serial.println("Not sure what happened");
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
  }
}
