#ifndef SineEase_h
#define SineEase_h

#include "Arduino.h"
#include "Ease.h"

#define SINE_IN 1
#define SINE_OUT 2
#define SINE_IN_OUT 3

class SineEase : public Ease
{
public:
	SineEase(int min, int max, unsigned long duration, bool repeat)
		: Ease(min, max, duration, repeat) 
	{
		if (repeat)
		{
			type = SINE_IN_OUT;
		}
		else if (Reversed)
		{
			type = SINE_OUT;
		}
		else 
		{
			type = SINE_IN;
		}
	}
	virtual unsigned long Calc(unsigned long currentTime);
protected:
	byte type;
};

#endif
