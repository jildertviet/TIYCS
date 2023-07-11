esp_now_peer_info_t slave;

bool parseOtaMsgAddressed(const uint8_t *data, int data_len, Mode m);

uint8_t replyAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t myAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

struct EspnowMsgParser{
public:
  static void receive(const uint8_t *mac_addr, const uint8_t *data, int data_len){
    if(!parseMain(mac_addr, data, data_len)){
      parseCustom(mac_addr, data, data_len);
    }
  };
  static bool parseMain(const uint8_t *mac_addr, const uint8_t *data, int data_len);
  static void parseCustom(const uint8_t *mac_addr, const uint8_t *data, int data_len);
  // Change behaviour in sub class
};

// EspnowMsgParser espnowMsgParser;

bool checkAddressed(const uint8_t* data){
  bool bAddressed = true;
  for(int i=0; i<6; i++){ // Check if this msg is for this ESP32
    if(myAddr[i] != data[i+1]){
      bAddressed = false;
    }
  }
  return bAddressed;
}


bool EspnowMsgParser::parseMain(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char msgType = data[0];
  if(msgType != 'a'){
    lastReceived = millis();
  }
  switch(msgType){
    case 0x03:
    Serial.println("Set ID");
      id = data[1];
      EEPROM.write(0, id);
      EEPROM.commit();
      Serial.println((int)id);
      break;
    case 0x07:{ // Connect with AP
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

    case 0x10: // [msgType, address[6], minutesToSleep[2]]
      if(checkAddressed(data)){
        memcpy(&minutesToSleep, data+6+1, 2);
        mode = Mode::DEEP_SLEEP;
      }
      break;
    case 0x11:{ // {0x11, a, d, d, r, e, s, 0x05} : set ESP32 with address 'addres' to ID 0x05
      if(checkAddressed(data)){
        uint8_t newMsg[data_len - 6];
        newMsg[0] = 0x03;
        memcpy(newMsg+1, data + 7, data_len - 6);
        EspnowMsgParser::receive(mac_addr, newMsg, data_len-6); // Recursively call this function to set the ID
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
        EspnowMsgParser::receive(mac_addr, newMsg, 2);
      }
    }
    break;
    case 0x15:{
      Serial.print("0x15");
      if(checkAddressed(data)){
        parseOtaMsgAddressed(data, data_len, Mode::START_OTA_SERVER);
      }
    }
    break;
    default:
      return false; // If nothing is found: return false, so parseCustom can be tried
    break;
  }
  return true;
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
  esp_now_register_recv_cb(EspnowMsgParser::receive);
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
