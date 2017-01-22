#include "ColWipeSelector.h"

unsigned long ColWipeSelector::getTimeOffset()
{
	float progress;
	if (Forwards)
	{
		progress = float(currentCol - minCol);
	}
	else
	{
		progress = float(maxCol - currentCol);
	}
	progress = progress / float(1 + maxCol - minCol);
	return Duration * progress;
}
