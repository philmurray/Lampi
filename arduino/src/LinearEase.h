#ifndef LinearEase_h
#define LinearEase_h

#include "Arduino.h"
#include "Ease.h"

class LinearEase: public Ease
{
  public:
    LinearEase(int min, int max, int duration, bool repeat)
      : Ease(min, max, duration, repeat) {}
    virtual int Calc(int currentTime);
};

#endif
