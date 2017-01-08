#include "Arduino.h"
#include "LinearEase.h"

int LinearEase::Calc(int currentTime)
{
  int t = adjustedTime(currentTime);
  
  int val = MinVal + (MaxVal - MinVal)*(float(t)/float(Duration));
  //Serial.println("currentTime: " + String(currentTime) + " t: " + String(t) + " val: " + String(val));
  return val;
}
