#ifndef RowWipeSelector_h
#define RowWipeSelector_h

#include "BlockSelector.h"
#include "Arduino.h"
#include "Ease.h"

class RowWipeSelector : public BlockSelector
{
  public:
    RowWipeSelector(byte minR, byte maxR, byte minC, byte maxC, unsigned long duration, bool forwards, bool r, unsigned long rp);
    virtual unsigned long getPinTime(unsigned long currentTime, byte pin);
  protected:
	bool Reverse;
	bool Forwards;
	unsigned long Duration;
	unsigned long ReversePadding;
	virtual unsigned long getTimeOffset();
};

#endif
