#ifndef RowWipeSelectorReverse_h
#define RowWipeSelectorReverse_h

#include "RowWipeSelector.h"
#include "Arduino.h"
#include "Ease.h"

class RowWipeSelectorReverse : public RowWipeSelector
{
public:
	RowWipeSelectorReverse(byte minR, byte maxR, byte minC, byte maxC, unsigned long duration, bool forwards, unsigned long rp)
		:RowWipeSelector(minR, maxR, minC, maxC, duration, forwards) {
		ReversePadding = rp;
	}
	virtual unsigned long getPinTime(unsigned long currentTime, byte pin);
protected:
	unsigned long ReversePadding;
};

#endif
