#ifndef PinState_h
#define PinState_h

#include "Arduino.h"

class PinState
{
  public:
    PinState();
    virtual void update (int pin);
};

#endif
