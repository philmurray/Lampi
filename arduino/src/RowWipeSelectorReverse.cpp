// 
// 
// 

#include "RowWipeSelectorReverse.h"

unsigned long RowWipeSelectorReverse::getPinTime(unsigned long currentTime, byte pin)
{
	unsigned long t = BlockSelector::getPinTime(currentTime, pin);
	if (t != long(-1))
	{
		t = ((Duration + ReversePadding) - getTimeOffset()) - t;
	}
	return t;
}
