#include "Arduino.h"
#include "StripState.h"
#include "State.h"
#include "StripStateStep.h"
#include <Adafruit_NeoPixel.h>

#define STRIP_PINS 60

StripState::StripState(Adafruit_NeoPixel* str, struct StripStateStep steps[], byte len)
 : State()
{
    strip = str;
    StepsLength = len;
    Steps = steps;
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
    
    for (int s = 0; s < StepsLength; s++)
    {
      unsigned long st = timeElapsed;
      StripStateStep step = Steps[s];
      if (step.StartTime <= st && step.EndTime > st)
      {
        st = st - step.StartTime;
        //Serial.println(String(timeElapsed) + " " + String(step.StartTime) + " " + String(st) + " " +  String(step.EndTime));
      
        
        //Serial.println("pin: " + String(pin));  
  
        unsigned long pt = st;
        if (step.Selector != NULL)
        {
          pt = step.Selector->getPinTime(st, pin);
        }
        
        if (pt <= st)
        {
          if (step.RedEase != NULL)
          {
            red += step.RedEase->Calc(pt);
          }
          if (step.BlueEase != NULL)
          {
            blue += step.BlueEase->Calc(pt);
          }
          if (step.GreenEase != NULL)
          {
            green += step.GreenEase->Calc(pt);
          }
          if (step.WhiteEase != NULL)
          {
            white += step.WhiteEase->Calc(pt);
            //Serial.println(String(pt) + " white set to: " + String(white));
          }
        }
      
        //Serial.println("pin: " + String(pin) + " red: " + String(red) + " green: " + String(green) + " blue: " + String(blue) + " white: " + String(white));
      }
    
    }
    strip->setPixelColor(pin, strip->Color(constrain(red, 0, 255), constrain(green, 0, 255), constrain(blue, 0, 255), constrain(white, 0, 255)));
  }
  strip->show();
}


