#include "Arduino.h"
#include "PinState.h"
#include "BlinkPinState.h"

BlinkPinState::BlinkPinState(unsigned long ra, unsigned long ot)
 : PinState()
{
  repeatAfter = ra;
  onTime = ot;
}

void BlinkPinState::update(int pin)
{
  PinState::update(pin);
  unsigned long t = millis() % repeatAfter;
  if (t <= onTime)
  {
    digitalWrite(pin, HIGH);
  }
  else
  {
    digitalWrite(pin, LOW);
  }
}
