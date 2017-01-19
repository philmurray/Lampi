#ifndef RowRunnerSelector_h
#define RowRunnerSelector_h

#include "BlockSelector.h"
#include "Arduino.h"
#include "Ease.h"

class RowRunnerSelector : public BlockSelector
{
public:
	RowRunnerSelector(int minR, int maxR, int minC, int maxC, unsigned long s, int l, int r, bool rev);
	virtual unsigned long getPinTime(unsigned long currentTime, int pin);
protected:
	unsigned long Speed;
	int Length;
	int Repeat;
  bool Reversed;
  unsigned long TotalDuration;
  unsigned long OnMidpoint;
};

#endif
