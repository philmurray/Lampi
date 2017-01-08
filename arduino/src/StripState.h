#ifndef StripState_h
#define StripState_h

#include <Adafruit_NeoPixel.h>
#include "Arduino.h"
#include "State.h"
#include "StripStateStep.h"

class StripState : public State
{
  public:
    StripState(Adafruit_NeoPixel* str, StripStateStep *steps, byte len);
    ~StripState();
    virtual void update ();
  protected:
    Adafruit_NeoPixel* strip;
    struct StripStateStep *Steps;
    byte StepsLength;
  private:
    int totalDuration;
    int TimeOffset (int pin, StripStateStep step);
};

#endif
