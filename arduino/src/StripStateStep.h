#ifndef StripStateStep_h
#define StripStateStep_h

#include "Ease.h"
#include "Selector.h"

struct StripStateStep
{
  unsigned long StartTime;
  unsigned long Duration;
  Selector* Selector;
  Ease* RedEase;
  Ease* BlueEase;
  Ease* GreenEase;
  Ease* WhiteEase;
};


#endif
