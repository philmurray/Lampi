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
		void reset(const struct StripStateStep steps[], byte len, float transitionLength);
	protected:
		void getPinColor(byte p, const struct StripStateStep steps[], byte stepsLength, unsigned long timeEl, int* r, int* g, int* b, int* w);
		void updateTime();

		float transitionProgress();
		Adafruit_NeoPixel* strip;

		const StripStateStep* Steps;
		byte StepsLength;

		float TransitionLength;
		const StripStateStep* TransitionSteps;
		byte TransitionStepsLength;
	private:
		unsigned long timeElapsed;
		unsigned long startTime;
		unsigned long transitionStartTime;
};

#endif
