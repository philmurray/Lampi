#ifndef PinState_h
#define PinState_h

#include "Arduino.h"
#include "State.h"

class PinState : public State
{
  public:
    PinState(int p);
    virtual void update ();
  protected:
    int pin;
};

#endif
