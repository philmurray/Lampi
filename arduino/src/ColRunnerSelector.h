#ifndef ColRunnerSelector_h
#define ColRunnerSelector_h

#include "RowRunnerSelector.h"
#include "Arduino.h"
#include "Ease.h"

class ColRunnerSelector : public RowRunnerSelector
{
public:
	ColRunnerSelector(int minR, int maxR, int minC, int maxC, unsigned long s, int l, int r, bool rev)
		: RowRunnerSelector(minR, maxR, minC, maxC, s, l, r, rev) {}
protected:
	virtual int getDimension();
};

#endif
