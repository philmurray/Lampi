#ifndef BlockSelector_h
#define BlockSelector_h

#include "Selector.h"
#include "Arduino.h"

#define STRIP_ROWS 6
#define STRIP_COLS 10

class BlockSelector : public Selector
{
  public:
    BlockSelector(int minR, int maxR, int minC, int maxC);
    virtual unsigned long getPinTime(unsigned long currentTime, int pin);
  protected:
    int Row(int pin);
    int Col(int pin);
    int minRow;
    int maxRow;
    int minCol;
    int maxCol;

    int currentRow;
    int currentCol;
};

#endif
