#ifndef State_h
#define State_h

#include "Arduino.h"

class State
{
  public:
    State();
    virtual void update();
    virtual void reset();
  protected:
    long startTime;
    long timeElapsed;
};

#endif
