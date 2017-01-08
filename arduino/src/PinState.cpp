#include "Arduino.h"
#include "State.h"
#include "PinState.h"

PinState::PinState(int p)
 : State()
{
  pin = p;
}

void PinState::update()
{
  State::update();
}
