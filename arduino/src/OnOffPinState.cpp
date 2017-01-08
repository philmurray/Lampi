#include "Arduino.h"
#include "PinState.h"
#include "OnOffPinState.h"

OnOffPinState::OnOffPinState(int p, bool b)
 : PinState(p)
{
  on = b;
  if (on) {
    digitalWrite(pin, HIGH);
  }
  else {
    digitalWrite(pin, LOW);
  }
}
