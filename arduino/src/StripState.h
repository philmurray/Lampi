#ifndef StripState_h
#define StripState_h

#include <Adafruit_NeoPixel.h>
#include "Arduino.h"
#include "State.h"
#include "StripStateStep.h"

class StripState : public State
{
  public:
    StripState(Adafruit_NeoPixel* str, struct StripStateStep steps[], byte len);
    virtual void update ();
  protected:
    Adafruit_NeoPixel* strip;
    StripStateStep* Steps;
    byte StepsLength;
  private:
    unsigned long totalDuration;
};

#endif
