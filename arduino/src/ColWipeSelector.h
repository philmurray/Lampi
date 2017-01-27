#ifndef ColWipeSelector_h
#define ColWipeSelector_h

#include "RowWipeSelector.h"
#include "Arduino.h"
#include "Ease.h"

class ColWipeSelector : public RowWipeSelector
{
public:
	ColWipeSelector(byte minR, byte maxR, byte minC, byte maxC, unsigned long duration, bool forwards)
		: RowWipeSelector(minR, maxR, minC, maxC, duration, forwards) {}
protected:
	virtual unsigned long getTimeOffset();
};

#endif
