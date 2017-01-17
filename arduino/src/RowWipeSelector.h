#ifndef RowWipeSelector_h
#define RowWipeSelector_h

#include "BlockSelector.h"
#include "Arduino.h"
#include "Ease.h"

class RowWipeSelector : public BlockSelector
{
  public:
    RowWipeSelector(int minR, int maxR, int minC, int maxC, Ease* e);
    virtual unsigned long getPinTime(unsigned long currentTime, int pin);
  protected:
    Ease* ease;
};

#endif