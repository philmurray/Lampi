#include "Arduino.h"
#include "StripState.h"
#include "StripStateStep.h"
#include <Adafruit_NeoPixel.h>

#define STRIP_PINS 60
#define REALLY_BIG 4000000000

StripState::StripState(Adafruit_NeoPixel* str, struct StripStateStep steps[], byte len)
{
    strip = str;
    StepsLength = len;
    Steps = steps;
}

void StripState::update()
{
	updateTime();
  //Serial.println("updating stripState" + String(timeElapsed));

  float percent = float(timeElapsed) / TRANSITION_LENGTH;
  float oldPercent = 1.0f - percent;

  if (Transition != NULL && percent < 1.0f) {
	  Transition->updateTime();
  }

  for (int pin = 0; pin < STRIP_PINS; pin++)
  {
    int red = 0;
    int green = 0;
    int blue = 0;
    int white = 0;

	getPinColor(pin, &red, &green, &blue, &white);

	if (percent < 1.0f )
	{
		int oldRed = 0;
		int oldGreen = 0;
		int oldBlue = 0;
		int oldWhite = 0;

		if (Transition != NULL)
		{
			Transition->getPinColor(pin, &oldRed, &oldGreen, &oldBlue, &oldWhite);
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

void StripState::reset(StripState * t)
{
	Transition = t;
	timeElapsed = 0;
	startTime = millis();
}

void StripState::getPinColor(int pin, int * red, int * green, int * blue, int * white)
{
	for (int s = 0; s < StepsLength; s++)
	{
		unsigned long st = timeElapsed;
		StripStateStep step = Steps[s];
		if (step.StartTime <= st && step.Duration > st - step.StartTime)
		{
			st = st - step.StartTime;
			//Serial.println(String(timeElapsed) + " " + String(step.StartTime) + " " + String(st) + " " +  String(step.EndTime));


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


