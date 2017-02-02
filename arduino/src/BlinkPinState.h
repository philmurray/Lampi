#ifndef BlinkPinState_h
#define BlinkPinState_h

#include "Arduino.h"
#include "PinState.h"

class BlinkPinState : public PinState
{
  public:
    BlinkPinState(unsigned long ra, unsigned long ot, unsigned long off);
    virtual void update(byte pin);
  protected:
    unsigned long repeatAfter;
    unsigned long onTime;
	unsigned long offset;
};

#endif
