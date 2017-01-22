#ifndef ColWipeSelector_h
#define ColWipeSelector_h

#include "RowWipeSelector.h"
#include "Arduino.h"
#include "Ease.h"

class ColWipeSelector : public RowWipeSelector
{
public:
	ColWipeSelector(int minR, int maxR, int minC, int maxC, unsigned long duration, bool forwards, bool r)
		: RowWipeSelector(minR, maxR, minC, maxC, duration, forwards, r) {}
protected:
	virtual unsigned long getTimeOffset();
};

#endif
