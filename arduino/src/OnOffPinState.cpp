#include "Arduino.h"
#include "PinState.h"
#include "OnOffPinState.h"

OnOffPinState::OnOffPinState(bool b)
 : PinState()
{
  on = b;
}

void OnOffPinState::update(byte pin)
{
  PinState::update(pin);
  if (on) {
    digitalWrite(pin, HIGH);
  }
  else {
    digitalWrite(pin, LOW);
  }
}

