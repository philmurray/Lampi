#ifndef StripState_h
#define StripState_h

#include <Adafruit_NeoPixel.h>
#include "Arduino.h"
#include "StripStateStep.h"

#define TRANSITION_LENGTH 750.0f

class StripState
{
	public:
		StripState(Adafruit_NeoPixel* str, struct StripStateStep steps[], byte len);
		void update ();
		void reset(StripState* t);
	protected:
		StripState* Transition;
		void getPinColor(byte p, int* r, int* g, int* b, int* w);
		void updateTime();
		float transitionProgress();
		Adafruit_NeoPixel* strip;
		StripStateStep* Steps;
		byte StepsLength;
	private:
		unsigned long timeElapsed;
		unsigned long startTime;
};

#endif
