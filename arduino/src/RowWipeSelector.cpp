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
  if (t >= 0)
  {
    return currentTime;
  }
}
