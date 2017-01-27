#ifndef RowWipeSelector_h
#define RowWipeSelector_h

#include "BlockSelector.h"
#include "Arduino.h"
#include "Ease.h"

class RowWipeSelector : public BlockSelector
{
  public:
    RowWipeSelector(byte minR, byte maxR, byte minC, byte maxC, unsigned long duration, bool forwards);
    virtual unsigned long getPinTime(unsigned long currentTime, byte pin);
  protected:
	bool Forwards;
	unsigned long Duration;
	virtual unsigned long getTimeOffset();
};

#endif
