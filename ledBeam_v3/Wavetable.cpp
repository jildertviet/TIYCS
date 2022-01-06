#include "Arduino.h" // For TWO_PI
#include "Wavetable.h"

Wavetable::Wavetable(int resolution){
  this->resolution = resolution;
  data = new float[resolution];
}

Wavetable::~Wavetable(){
  delete data;
}

void Wavetable::fillSine(float power){
  for(int i=0; i<resolution; i++){
    float pct = i / (float)resolution;
    data[i] = sin(TWO_PI * pct) * 0.5 + 0.5; // Normalized
    data[i] = pow(data[i], power);
  }
}

float Wavetable::getValue(float pct){
  return data[(int)(pct * resolution)];
}
