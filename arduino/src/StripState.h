#ifndef StripState_h
#define StripState_h

#include <Adafruit_NeoPixel.h>
#include "Arduino.h"
#include "StripStateStep.h"

class StripState
{
  public:
    StripState(Adafruit_NeoPixel* str, struct StripStateStep steps[], byte len);
    void update ();
    void reset();
  protected:
    Adafruit_NeoPixel* strip;
    StripStateStep* Steps;
    byte StepsLength;
  private:
    unsigned long startTime;
    unsigned long timeElapsed;
};

#endif
