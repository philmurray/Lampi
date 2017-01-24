#ifndef StripState_h
#define StripState_h

#include <Adafruit_NeoPixel.h>
#include "Arduino.h"
#include "StripStateStep.h"

class StripState
{
	public:
		StripState(Adafruit_NeoPixel* str);
		void update ();
		void reset(struct StripStateStep steps[], byte len, float transitionLength);
	protected:
		void getPinColor(byte p, struct StripStateStep steps[], byte stepsLength, unsigned long timeEl, int* r, int* g, int* b, int* w);
		void updateTime();

		float transitionProgress();
		Adafruit_NeoPixel* strip;

		StripStateStep* Steps;
		byte StepsLength;

		float TransitionLength;
		StripStateStep* TransitionSteps;
		byte TransitionStepsLength;
	private:
		unsigned long timeElapsed;
		unsigned long startTime;
		unsigned long transitionStartTime;
};

#endif
