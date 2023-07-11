
void EspnowMsgParser::parseCustom(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char msgType = data[0];
  switch(msgType){
    case 0x05:
      memcpy(values, data + 1 + (id*4), 4);
      mode = NOLAG;
      bUpdate = true;
      break;
    case 0x09: // [msgType, lagTime, R, G, B, W, R, G, B, W, etc]
     mode = LAG;
     memcpy(&lagTime, data+1, 2); // Copy the last two char's, and write to unsigned short.
     memcpy(endValues, data + 1 + 2 + (id*4), 4);
     memcpy(startValues, values, 4);
     envStartTime = millis();
     envEndTime = millis() + lagTime;
     bLagDone = false;
    break;
    case 0x08: // Reply with battery voltage
      if(checkAddressed(data)){
        mode = Mode::SEND_BATTERY;
      }
      break;
    break;
  }
}
