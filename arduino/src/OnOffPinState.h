#ifndef OnOffPinState_h
#define OnOffPinState_h

#include "Arduino.h"
#include "PinState.h"

class OnOffPinState : public PinState
{
  public:
    OnOffPinState(int p, bool o);
  protected:
    bool on;
};

#endif
