#include "Arduino.h"
#include "PinState.h"
#include "BlinkPinState.h"
#include "State.h"

BlinkPinState::BlinkPinState(int p, unsigned long ra, unsigned long ot)
 : PinState(p)
{
  repeatAfter = ra;
  onTime = ot;
}

void BlinkPinState::update()
{
  PinState::update();
  unsigned long t = timeElapsed % repeatAfter;
  if (t <= onTime)
  {
    digitalWrite(pin, HIGH);
  }
  else
  {
    digitalWrite(pin, LOW);
  }
}
