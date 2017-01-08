#ifndef Ease_h
#define Ease_h

#include "Arduino.h"

class Ease
{
  public:
    Ease(int min, int max, int duration, bool repeat);
    virtual int Calc(int currentTime);
  protected:
    int adjustedTime(int currentTime);
    bool Repeat;
    int MinVal;
    int MaxVal;
    int Duration;
    int Reversed;
};

#endif
