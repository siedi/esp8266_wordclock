#ifndef EWC_TIME_H
#define EWC_TIME_H

#include <time.h>
#include <Timezone.h> // https://github.com/vanceb/Timezone !!!
#include "EWCConfig.h"

class EWCTime
{
  public:
    EWCTime();
    ~EWCTime();
    void begin();
    time_t now();

  private:
    TimeChangeRule _tcrSummertime;
    TimeChangeRule _tcrWintertime;
    Timezone _Tz;

};

extern EWCTime Time;

#endif
