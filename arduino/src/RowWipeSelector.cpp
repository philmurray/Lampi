#include "Arduino.h"
#include "RowWipeSelector.h"
#include "BlockSelector.h"

RowWipeSelector::RowWipeSelector(byte minR, byte maxR, byte minC, byte maxC, unsigned long duration, bool forwards)
 : BlockSelector(minR, maxR, minC, maxC)
{
  Forwards = forwards;
  Duration = duration;
}

unsigned long RowWipeSelector::getPinTime(unsigned long currentTime, byte pin)
{
  unsigned long t = BlockSelector::getPinTime(currentTime, pin);
  if (t != long(-1))
  {
	t = t - getTimeOffset();
  }
  return t;
}

unsigned long RowWipeSelector::getTimeOffset()
{
	float progress;
	if (Forwards)
	{
		progress = float(currentRow - minRow);
	}
	else
	{
		progress = float(maxRow - currentRow);
	}
	progress = progress / float(1 + maxRow - minRow);
	return Duration * progress;
}
