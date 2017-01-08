#include "Arduino.h"
#include "PinState.h"
#include "BlinkPinState.h"
#include "State.h"

BlinkPinState::BlinkPinState(int p, int ra, int ot)
 : PinState(p)
{
  repeatAfter = ra;
  onTime = ot;
}

void BlinkPinState::update()
{
  PinState::update();
  long t = timeElapsed % repeatAfter;
  if (t <= onTime)
  {
    digitalWrite(pin, HIGH);
  }
  else
  {
    digitalWrite(pin, LOW);
  }
}
