#ifndef ColWipeSelector_h
#define ColWipeSelector_h

#include "BlockSelector.h"
#include "Arduino.h"
#include "Ease.h"

class ColWipeSelector : public BlockSelector
{
public:
	ColWipeSelector(int minR, int maxR, int minC, int maxC, Ease* e);
	virtual unsigned long getPinTime(unsigned long currentTime, int pin);
protected:
	Ease* ease;
};

#endif
