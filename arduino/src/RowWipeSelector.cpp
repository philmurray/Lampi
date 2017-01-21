#include "Arduino.h"
#include "RowWipeSelector.h"
#include "BlockSelector.h"

RowWipeSelector::RowWipeSelector(int minR, int maxR, int minC, int maxC, Ease* e, bool r)
 : BlockSelector(minR, maxR, minC, maxC)
{
  ease = e;
  Reverse = r;
}

unsigned long RowWipeSelector::getPinTime(unsigned long currentTime, int pin)
{
  unsigned long t = BlockSelector::getPinTime(currentTime, pin);
  if (t != long(-1))
  {
	  if (Reverse) {
			//Do something clever
	  }
	  t = t - ease->Calc(getDimensionDifference());
  }
  return t;
}

int RowWipeSelector::getDimensionDifference()
{
	return currentRow - minRow;
}
