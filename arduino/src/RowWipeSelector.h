#ifndef RowWipeSelector_h
#define RowWipeSelector_h

#include "BlockSelector.h"
#include "Arduino.h"
#include "Ease.h"

class RowWipeSelector : public BlockSelector
{
  public:
    RowWipeSelector(int minR, int maxR, int minC, int maxC, unsigned long duration, bool forwards, bool r);
    virtual unsigned long getPinTime(unsigned long currentTime, int pin);
  protected:
	bool Reverse;
	bool Forwards;
	unsigned long Duration;
	virtual unsigned long getTimeOffset();
};

#endif
