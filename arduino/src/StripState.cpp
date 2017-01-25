#include "Arduino.h"
#include "StripState.h"
#include "StripStateStep.h"
#include <Adafruit_NeoPixel.h>

#define STRIP_PINS 60
#define REALLY_BIG 4000000000

StripState::StripState(Adafruit_NeoPixel* str)
{
	strip = str;
}

void StripState::update()
{
	updateTime();
	//Serial.println("updating stripState" + String(timeElapsed));
	float percent = transitionProgress();
	float oldPercent = 1.0f - percent;
	unsigned long transitionElapsed;

	if (TransitionSteps != NULL && percent < 1.0f) {
		transitionElapsed = millis() - transitionStartTime;
	}
	else
	{
		TransitionSteps = NULL;
	}

	for (byte pin = 0; pin < STRIP_PINS; pin++)
	{
		int red = 0;
		int green = 0;
		int blue = 0;
		int white = 0;

		getPinColor(pin, Steps, StepsLength, timeElapsed, &red, &green, &blue, &white);

		if (percent < 1.0f)
		{
			int oldRed = 0;
			int oldGreen = 0;
			int oldBlue = 0;
			int oldWhite = 0;

			if (TransitionSteps != NULL)
			{
				getPinColor(pin, TransitionSteps, TransitionStepsLength, transitionElapsed, &oldRed, &oldGreen, &oldBlue, &oldWhite);
			}
			red = red*percent + oldRed*oldPercent;
			green = green*percent + oldGreen*oldPercent;
			blue = blue*percent + oldBlue*oldPercent;
			white = white*percent + oldWhite*oldPercent;
		}

		strip->setPixelColor(pin, strip->Color(constrain(red, 0, 255), constrain(green, 0, 255), constrain(blue, 0, 255), constrain(white, 0, 255)));
	}
	strip->show();
}

void StripState::reset(const struct StripStateStep steps[], byte len, float transitionLength)
{
	if (transitionLength > 0.0f && transitionProgress() > 0.5f)
	{
		TransitionSteps = Steps;
		TransitionStepsLength = StepsLength;
		transitionStartTime = startTime;
	}
	Steps = steps;
	StepsLength = len;

	TransitionLength = transitionLength;

	timeElapsed = 0;
	startTime = millis();
}

void StripState::getPinColor(byte pin, const struct StripStateStep steps[], byte stepsLength, unsigned long timeEl, int * red, int * green, int * blue, int * white)
{

	for (byte s = 0; s < stepsLength; s++)
	{
		unsigned long st = timeEl;
		StripStateStep step;

		memcpy_P(&step, &steps[s], sizeof(step));

		if (step.StartTime <= st && step.Duration > st - step.StartTime)
		{
			st = st - step.StartTime;


			//Serial.println("pin: " + String(pin));  

			unsigned long pt = st;
			if (step.Selector != NULL)
			{
				pt = step.Selector->getPinTime(st, pin);
			}

			if (pt < REALLY_BIG)
			{
				if (step.RedEase != NULL)
				{
					*red += step.RedEase->Calc(pt);
				}
				if (step.BlueEase != NULL)
				{
					*blue += step.BlueEase->Calc(pt);
				}
				if (step.GreenEase != NULL)
				{
					*green += step.GreenEase->Calc(pt);
				}
				if (step.WhiteEase != NULL)
				{
					*white += step.WhiteEase->Calc(pt);
					//Serial.println(String(pt) + " white set to: " + String(white));
				}
			}

			//Serial.println("pin: " + String(pin) + " red: " + String(red) + " green: " + String(green) + " blue: " + String(blue) + " white: " + String(white));
		}

	}
}

void StripState::updateTime()
{
	timeElapsed = millis() - startTime;
}

float StripState::transitionProgress()
{
	if (TransitionLength > 0.0f)
	{
		return float(timeElapsed) / TransitionLength;
	}
	else
	{
		return 1.0f;
	}
}


