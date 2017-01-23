#ifndef Selector_h
#define Selector_h

#include "Arduino.h"

class Selector
{
  public:
    Selector();
    virtual unsigned long getPinTime(unsigned long currentTime, byte pin);
};

#endif
