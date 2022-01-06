#ifndef WAVETABLE_J
#define WAVETABLE_J

class Wavetable{
  public:
  Wavetable(int resolution);
  ~Wavetable();
  int resolution;
  void fillSine(float power = 1);
  float* data;
  float getValue(float pct);
};

#endif
