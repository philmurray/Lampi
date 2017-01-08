#include "Arduino.h"
#include "State.h"

State::State()
{
  reset();
}

void State::update()
{
  timeElapsed = millis() - startTime;
}

void State::reset()
{
  timeElapsed = 0;
  startTime = millis();
}
