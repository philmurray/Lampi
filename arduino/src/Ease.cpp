#include "Arduino.h"
#include "Ease.h"

Ease::Ease(int min, int max, int duration, bool repeat)
{
  MinVal = min;
  MaxVal = max;
  Duration = duration;
  Repeat = repeat;
  if (MinVal > MaxVal)
  {
    Reversed = true;
    MinVal = max;
    MaxVal = min;
  }
}

int Ease::Calc(int currentTime)
{
  
}

int Ease::adjustedTime(int currentTime)
{
  int t;
  if (Repeat) {
    t = currentTime % Duration;
  }
  else
  {
    t = constrain(currentTime, 0, Duration);
  }
  if (Reversed)
  {
    t = Duration - t;
  }
  //Serial.println(" currentTime: " + String(currentTime) + " reversed " + String(Reversed) + " Duration " + String(Duration) + " t " + String(t) );
  return t;
}

