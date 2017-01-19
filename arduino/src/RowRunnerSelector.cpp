#include "Arduino.h"
#include "RowRunnerSelector.h"
#include "BlockSelector.h"

RowRunnerSelector::RowRunnerSelector(int minR, int maxR, int minC, int maxC, unsigned long s, int l, int r, bool rev)
	: BlockSelector(minR, maxR, minC, maxC)
{
	Speed = s;
	Length = l;
	Repeat = r;
  Reversed = rev;

  TotalDuration = Speed * Repeat;
  OnMidpoint = (Speed * Length) / 2;
}

unsigned long RowRunnerSelector::getPinTime(unsigned long currentTime, int pin)
{
	unsigned long t = BlockSelector::getPinTime(currentTime, pin);
	if (t != long(-1))
	{
		t = t % (TotalDuration);
		if (Reversed)
		{
		  t = TotalDuration - t;
		}
		int maxR = t / Speed;
		int minR = ((maxR - Length) + 1 + Repeat) % Repeat;

		int r = currentRow % Repeat;
		if ((minR <= maxR && r >= minR && r <= maxR) || (minR > maxR && (r <= maxR || r >= minR)))
		{
			t = (t + TotalDuration - (r * Speed)) % TotalDuration;
			if ( t >= OnMidpoint) {
				t = (OnMidpoint * 2 - t);
			}
		}
		else
		{
			t = -1;
		}
		
	}
	return t;
}
