#include "Arduino.h"
#include "SineEase.h"

unsigned long SineEase::Calc(unsigned long currentTime)
{
	unsigned long t = adjustedTime(currentTime);

	if (type == SINE_IN) {
		return -(MaxVal - MinVal)*cos(float(t) / float(Duration)*M_PI_2) + MaxVal;
	} else if (type == SINE_OUT) {
		return (MaxVal - MinVal)*sin(float(t) / float(Duration)*M_PI_2) + MinVal;
	} else if (type == SINE_IN_OUT) {
		return -(MaxVal - MinVal) / 2 * (cos(M_PI*float(t) / float(Duration)) - 1) + MinVal;
	}
}
