
#ifdef  IS_JONISK
  #define R_PIN 13
  #define G_PIN 12
  #define B_PIN 22
  #define W_PIN 23
#endif

#ifdef IS_JONISK_2022_EXTRA
  void initPins(){
    pinMode(34, INPUT);
    pinMode(14, INPUT);
    // pinMode(2, INPUT_PULLUP); // Fix for board that has pin2 shorted to 15...
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
  }
  #define R_PIN 16
  #define G_PIN 17
  #define B_PIN 18
  #define W_PIN 19
#endif

#ifdef IS_LED // Used for ESP32S2 boards?
  #define R_PIN 16
  #define G_PIN 33
  #define B_PIN 17
  #define W_PIN 32
#endif

#ifdef IS_LED_FIXBOARD // Used for ESP32S2 with fixBoard on top (these are thrown away!)
  #define R_PIN 14
  #define G_PIN 15
  #define B_PIN 16
  #define W_PIN 17
#endif

#ifdef IS_JONISK_NEW
  #define R_PIN 46
  #define G_PIN 45
  #define B_PIN 41
  #define W_PIN 42
#endif

#ifdef IS_JONISK_2022
  #define R_PIN 16
  #define G_PIN 17
  #define B_PIN 18
  #define W_PIN 19
  void initPins(){
    pinMode(34, INPUT);
    pinMode(14, INPUT);
    pinMode(15, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP); // Fix for board that has pin2 shorted to 15...
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
  }
#endif

#ifdef IS_PONTLED_2019
  #define R_PIN 16
  #define G_PIN 33
  #define B_PIN 17
  #define W_PIN 32
  void initPins(){};
#endif

#ifdef IS_PONTLED_2022_V1
  #define R_PIN 12
  #define G_PIN 13
  #define B_PIN 22
  #define W_PIN 23
  void initPins(){
    // pinMode(5, OUTPUT);
  }
#endif
