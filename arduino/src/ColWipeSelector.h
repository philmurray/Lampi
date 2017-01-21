#ifndef ColWipeSelector_h
#define ColWipeSelector_h

#include "RowWipeSelector.h"
#include "Arduino.h"
#include "Ease.h"

class ColWipeSelector : public RowWipeSelector
{
public:
	ColWipeSelector(int minR, int maxR, int minC, int maxC, Ease* e)
		: RowWipeSelector(minR, maxR, minC, maxC, e) {}
protected:
	virtual int getDimensionDifference();
};

#endif
