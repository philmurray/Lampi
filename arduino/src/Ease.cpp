#include "Arduino.h"
#include "Ease.h"

Ease::Ease(int min, int max, unsigned long duration, bool repeat)
{
  MinVal = min;
  MaxVal = max;
  Duration = duration;
  Repeat = repeat;
}

int Ease::Calc(unsigned long currentTime)
{
  return MaxVal;
}

unsigned long Ease::adjustedTime(unsigned long currentTime)
{
  unsigned long t;
  if (Repeat) 
  {
	  t = currentTime % (Duration * 2);
	  if (t > Duration)
	  {
		  t = Duration * 2 - t;
	  }
  }
  else
  {
    t = constrain(currentTime, 0, Duration);
  }
  //Serial.println(" currentTime: " + String(currentTime) + " reversed " + String(Reversed) + " Duration " + String(Duration) + " t " + String(t) );
  return t;
}

