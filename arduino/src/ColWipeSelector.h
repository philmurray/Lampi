#ifndef ColWipeSelector_h
#define ColWipeSelector_h

#include "RowWipeSelector.h"
#include "Arduino.h"
#include "Ease.h"

class ColWipeSelector : public RowWipeSelector
{
public:
	ColWipeSelector(byte minR, byte maxR, byte minC, byte maxC, unsigned long duration, bool forwards, bool r, unsigned long rp)
		: RowWipeSelector(minR, maxR, minC, maxC, duration, forwards, r, rp) {}
protected:
	virtual unsigned long getTimeOffset();
};

#endif
