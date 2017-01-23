#ifndef BlockSelector_h
#define BlockSelector_h

#include "Selector.h"
#include "Arduino.h"

#define STRIP_ROWS 6
#define STRIP_COLS 10

class BlockSelector : public Selector
{
  public:
    BlockSelector(byte minR, byte maxR, byte minC, byte maxC);
    virtual unsigned long getPinTime(unsigned long currentTime, byte pin);
  protected:
	  byte Row(byte pin);
	  byte Col(byte pin);
	  byte minRow;
	  byte maxRow;
	  byte minCol;
	  byte maxCol;

	  byte currentRow;
	  byte currentCol;
};

#endif
