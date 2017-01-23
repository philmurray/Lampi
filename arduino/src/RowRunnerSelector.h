#ifndef RowRunnerSelector_h
#define RowRunnerSelector_h

#include "BlockSelector.h"
#include "Arduino.h"
#include "Ease.h"

class RowRunnerSelector : public BlockSelector
{
public:
	RowRunnerSelector(byte minR, byte maxR, byte minC, byte maxC, unsigned long s, byte l, byte r, bool rev);
	virtual unsigned long getPinTime(unsigned long currentTime, byte pin);
protected:
	unsigned long Speed;
	byte Length;
	byte Repeat;
	bool Reversed;
	unsigned long TotalDuration;
	unsigned long OnMidpoint;

	virtual byte getDimension();
};

#endif
