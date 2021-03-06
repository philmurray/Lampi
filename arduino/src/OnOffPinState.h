#ifndef OnOffPinState_h
#define OnOffPinState_h

#include "Arduino.h"
#include "PinState.h"

class OnOffPinState : public PinState
{
  public:
    OnOffPinState(bool o);
    virtual void update (byte pin);
  protected:
    bool on;
};

#endif
