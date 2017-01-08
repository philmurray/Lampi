#include "Arduino.h"
#include "StripState.h"
#include "State.h"
#include "StripStateStep.h"
#include <Adafruit_NeoPixel.h>

#define STRIP_PINS 60

StripState::StripState(Adafruit_NeoPixel* str, StripStateStep *steps, byte len)
 : State()
{
    strip = str;
    StepsLength = len;
    Steps = malloc(StepsLength * sizeof(StripStateStep));
    byte i;
    
    for (i = 0; i < StepsLength; i++)
    {
        Steps[i] = steps[i];
        totalDuration = max(totalDuration, Steps[i].EndTime);
    }
}

StripState::~StripState()
{
  free(Steps);
}

void StripState::update()
{
  State::update();
  //Serial.println("updating stripState" + String(timeElapsed));
  
  for (int pin = 0; pin < STRIP_PINS; pin++)
  {
    int red = 0;
    int green = 0;
    int blue = 0;
    int white = 0;

    //Serial.println("pin: " + String(pin));

    for (int s = 0; s < StepsLength; s++)
    {
      StripStateStep step = Steps[s];
      //Serial.println(String(step.StartTime) + " " + String(timeElapsed) + " " +  String(step.EndTime));
      if (step.StartTime <= timeElapsed && (step.EndTime == -1 || step.EndTime > timeElapsed))
      {
        int t = TimeOffset(pin, step);
        if (step.RedEase != NULL)
        {
          red += step.RedEase->Calc(t);
        }
        if (step.BlueEase != NULL)
        {
          blue += step.BlueEase->Calc(t);
        }
        if (step.GreenEase != NULL)
        {
          green += step.GreenEase->Calc(t);
        }
        if (step.WhiteEase != NULL)
        {
          white += step.WhiteEase->Calc(t);
          //Serial.println(String(t) + " white set to: " + String(white));
        }
      }
    }

    //Serial.println("red: " + String(red) + "green: " + String(green) + "blue: " + String(blue) + "white: " + String(white));
    strip->setPixelColor(pin, strip->Color(constrain(red, 0, 255), constrain(green, 0, 255), constrain(blue, 0, 255), constrain(white, 0, 255)));
  }
  strip->show();
}

int StripState::TimeOffset (int pin, StripStateStep step)
{
  //Serial.println("TimeOffset: " + String(timeElapsed));
  return timeElapsed;
}


