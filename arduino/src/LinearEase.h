#ifndef LinearEase_h
#define LinearEase_h

#include "Arduino.h"
#include "Ease.h"

class LinearEase: public Ease
{
  public:
    LinearEase(int min, int max, unsigned long duration, bool repeat)
      : Ease(min, max, duration, repeat) {}
    virtual unsigned long Calc(unsigned long currentTime);
};

#endif
