#include "Arduino.h"
#include "Selector.h"
#include "BlockSelector.h"

BlockSelector::BlockSelector(byte minR, byte maxR, byte minC, byte maxC)
  : Selector()
{
  minRow = minR;
  maxRow = maxR;
  minCol = minC;
  maxCol = maxC;
}

unsigned long BlockSelector::getPinTime(unsigned long currentTime, byte pin)
{
  currentRow = Row(pin);
  currentCol = Col(pin);
  if (currentRow >= minRow && currentRow <= maxRow && currentCol >= minCol && currentCol <= maxCol) {
    //Serial.prbyteln(String(pin) + " row: " + String(currentRow) + " col: " + String(currentCol) + " included");
    return currentTime;
  }
    //Serial.prbyteln(String(pin) + " row: " + String(currentRow) + " col: " + String(currentCol) + " not included");
  return -1;
}

byte BlockSelector::Row(byte pin)
{
  return pin / STRIP_COLS;
}

byte BlockSelector::Col(byte pin)
{
  return pin % STRIP_COLS;
}
