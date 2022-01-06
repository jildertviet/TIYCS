#ifndef pEnv_h
#define pEnv_h

#include "Arduino.h"

class pEnv{
public:
    pEnv();
    float update();
    bool bActive = false;
    unsigned long stopTime = 0;
    unsigned long startTime = 0;
    unsigned short duration = 0;
    unsigned short a, s, r;
    float brightness = 0;
    void trigger(unsigned short a, unsigned short s, unsigned short r, float b);
    float value = 0;
    bool stateRandom = false;
};

#endif  // pEnv_h
