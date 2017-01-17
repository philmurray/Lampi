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
    unsigned long startTime;
    unsigned long timeElapsed;
};

#endif
