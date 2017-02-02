#include "Arduino.h"
#include "PinState.h"
#include "BlinkPinState.h"

BlinkPinState::BlinkPinState(unsigned long ra, unsigned long ot, unsigned long off)
 : PinState()
{
  repeatAfter = ra;
  onTime = ot;
  offset = off;
}

void BlinkPinState::update(byte pin)
{
  PinState::update(pin);
  unsigned long t = (millis() + offset) % repeatAfter;
  if (t <= onTime)
  {
    digitalWrite(pin, HIGH);
  }
  else
  {
    digitalWrite(pin, LOW);
  }
}
