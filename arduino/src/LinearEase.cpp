#include "Arduino.h"
#include "LinearEase.h"

int LinearEase::Calc(unsigned long currentTime)
{
  //Serial.println("currentTime: " + String(currentTime) + " t: " + String(t) + " val: " + String(val) + " reversed: " + String(Reversed));
  return MinVal + (MaxVal - MinVal)*(float(adjustedTime(currentTime)) / float(Duration));
}
