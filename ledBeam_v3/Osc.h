#ifndef OSC_J
#define OSC_J

#include "Wavetable.h"
#include "Arduino.h"

enum BlendMode{
  BLEND_NONE,
  BLEND_MAX,
  BLEND_TEST
};

class Osc{
  public:
  Osc(Wavetable* w);
  Wavetable* w;
  void setWavetable(Wavetable* w);
  void update();
  void write(float* data, int num, BlendMode=BLEND_NONE);
  float phase;
  float range = 1;
  float offset = 0;
  float frequency = 0.1;
  void start();
  double startTime = 0;
  bool bActive = false;
};

#endif
