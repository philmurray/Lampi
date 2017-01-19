#include "Arduino.h"
#include "ColWipeSelector.h"
#include "BlockSelector.h"

ColWipeSelector::ColWipeSelector(int minR, int maxR, int minC, int maxC, Ease* e)
	: BlockSelector(minR, maxR, minC, maxC)
{
	ease = e;
}

unsigned long ColWipeSelector::getPinTime(unsigned long currentTime, int pin)
{
	unsigned long t = BlockSelector::getPinTime(currentTime, pin);
	if (t != long(-1))
	{
		t = t - ease->Calc(currentCol - minCol);
	}
	return t;
}
