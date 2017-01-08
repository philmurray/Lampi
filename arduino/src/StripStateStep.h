#ifndef StripStateStep_h
#define StripStateStep_h

#include "Ease.h"

struct StripStateStep
{
  int StartTime;
  int EndTime;
  int MinRow;
  int MaxRow;
  int MinCol;
  int MaxCol;
  Ease* RowEase;
  Ease* ColEase;
  Ease* RedEase;
  Ease* BlueEase;
  Ease* GreenEase;
  Ease* WhiteEase;
};


#endif
