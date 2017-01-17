#ifndef Ease_h
#define Ease_h

#include "Arduino.h"

class Ease
{
  public:
    Ease(int min, int max, unsigned long duration, bool repeat);
    virtual unsigned long Calc(unsigned long currentTime);
  protected:
    unsigned long adjustedTime(unsigned long currentTime);
    bool Repeat;
    int MinVal;
    int MaxVal;
    unsigned long Duration;
    bool Reversed;
};

#endif