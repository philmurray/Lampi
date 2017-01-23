#ifndef ColRunnerSelector_h
#define ColRunnerSelector_h

#include "RowRunnerSelector.h"
#include "Arduino.h"
#include "Ease.h"

class ColRunnerSelector : public RowRunnerSelector
{
public:
	ColRunnerSelector(byte minR, byte maxR, byte minC, byte maxC, unsigned long s, byte l, byte r, bool rev)
		: RowRunnerSelector(minR, maxR, minC, maxC, s, l, r, rev) {}
protected:
	virtual byte getDimension();
};

#endif
