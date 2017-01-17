#ifndef BlinkPinState_h
#define BlinkPinState_h

#include "Arduino.h"
#include "PinState.h"
#include "State.h"

class BlinkPinState : public PinState
{
  public:
    BlinkPinState(int p, unsigned long ra, unsigned long ot);
    virtual void update();
  protected:
    unsigned long repeatAfter;
    unsigned long onTime;
};

#endif
