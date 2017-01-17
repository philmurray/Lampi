#include "Arduino.h"
#include "Selector.h"
#include "BlockSelector.h"

BlockSelector::BlockSelector(int minR, int maxR, int minC, int maxC)
  : Selector()
{
  minRow = minR;
  maxRow = maxR;
  minCol = minC;
  maxCol = maxC;
}

unsigned long BlockSelector::getPinTime(unsigned long currentTime, int pin)
{
  currentRow = Row(pin);
  currentCol = Col(pin);
  if (currentRow >= minRow && currentRow <= maxRow && currentCol >= minCol && currentCol <= maxCol) {
    //Serial.println(String(pin) + " row: " + String(currentRow) + " col: " + String(currentCol) + " included");
    return currentTime;
  }
    //Serial.println(String(pin) + " row: " + String(currentRow) + " col: " + String(currentCol) + " not included");
  return -1;
}

int BlockSelector::Row(int pin)
{
  return pin / STRIP_COLS;
}

int BlockSelector::Col(int pin)
{
  return pin % STRIP_COLS;
}
