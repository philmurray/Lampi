#include "Arduino.h"
#include "RowRunnerSelector.h"
#include "BlockSelector.h"

RowRunnerSelector::RowRunnerSelector(byte minR, byte maxR, byte minC, byte maxC, unsigned long s, byte l, byte r, bool rev)
	: BlockSelector(minR, maxR, minC, maxC)
{
	Speed = s;
	Length = l;
	Repeat = r;
	Reversed = rev;

	TotalDuration = Speed * Repeat;
	OnMidpoint = (Speed * Length) / 2;
}

unsigned long RowRunnerSelector::getPinTime(unsigned long currentTime, byte pin)
{
	unsigned long t = BlockSelector::getPinTime(currentTime, pin);
	if (t != long(-1))
	{
		t = t % (TotalDuration);
		if (Reversed)
		{
		  t = TotalDuration - t;
		}
		byte maxR = t / Speed;
		byte minR = ((maxR - Length) + 1 + Repeat) % Repeat;

		byte r = getDimension() % Repeat;
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

byte RowRunnerSelector::getDimension()
{
	return currentRow;
}


