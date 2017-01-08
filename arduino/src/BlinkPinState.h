#ifndef BlinkPinState_h
#define BlinkPinState_h

#include "Arduino.h"
#include "PinState.h"
#include "State.h"

class BlinkPinState : public PinState
{
  public:
    BlinkPinState(int p, int ra, int ot);
    virtual void update();
  protected:
    int repeatAfter;
    int onTime;
};

#endif
