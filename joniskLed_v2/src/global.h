unsigned char id = 0; // Read from EEPROM
bool bUpdate = false;
unsigned long lastReceived = 0;
unsigned short minutesToSleep = 0;

void blinkLed(int channel, int delayTime, int num, int brightness=50);
void setLED(int channel, int value);
