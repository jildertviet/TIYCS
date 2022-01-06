#include "Osc.h"

Osc::Osc(Wavetable* w){
  this->w = w;
}

void Osc::update(){
  double t = (millis() - startTime) / 1000.; // To seconds
  phase = t * frequency;
  phase = fmod(phase, 1.0);
}

void Osc::start(){
  bActive = true;
  startTime = millis();
}

void Osc::write(float* data, int num, BlendMode blendMode){
  for(int i=0; i<num; i++){
    float pct = i / (float)num;
    pct *= range;
    pct += offset + phase;
    pct = fmod(pct, 1.0);
    switch(blendMode){
      case BLEND_NONE:{
        data[i] = w->getValue(pct); // Get by pct (0.0 - 0.99999)
      }
      break;
      case BLEND_MAX:{
        float value = w->getValue(pct);
        if(data[i] < value){
          data[i] = value;
        }
      }
      break;
      case BLEND_TEST:{
        if(i%10==0){
          data[i] = 1;
        } else{
          data[i] = 0;
        }
      }
    }
  }
}

void Osc::setWavetable(Wavetable* w){
  this->w = w;
}
