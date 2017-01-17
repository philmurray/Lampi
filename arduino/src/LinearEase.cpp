#include "Arduino.h"
#include "LinearEase.h"

unsigned long LinearEase::Calc(unsigned long currentTime)
{
  unsigned long t = adjustedTime(currentTime);
  
  int val = MinVal + (MaxVal - MinVal)*(float(t)/float(Duration));
  //Serial.println("currentTime: " + String(currentTime) + " t: " + String(t) + " val: " + String(val) + " reversed: " + String(Reversed));
  return val;
}
