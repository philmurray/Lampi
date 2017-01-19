#include "Arduino.h"
#include "RowWipeSelector.h"
#include "BlockSelector.h"

RowWipeSelector::RowWipeSelector(int minR, int maxR, int minC, int maxC, Ease* e)
 : BlockSelector(minR, maxR, minC, maxC)
{
  ease = e;
}

unsigned long RowWipeSelector::getPinTime(unsigned long currentTime, int pin)
{
  unsigned long t = BlockSelector::getPinTime(currentTime, pin);
  if (t != long(-1))
  {
	  t = t - ease->Calc(currentRow - minRow);
  }
  return t;
}
