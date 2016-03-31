#include "EWCTime.h"

EWCTime::EWCTime() :
  _tcrSummertime(),
  _tcrWintertime(),
  _Tz(_tcrSummertime, _tcrWintertime)
{

}

EWCTime::~EWCTime()
{
  
}

void EWCTime::begin()
{
  configTime(0, 0, const_cast<char *>(NTP_SERVER));

  //Reinitialize with Central European Time (Frankfurt, Paris)
  TimeChangeRule tcr1 = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
  _tcrSummertime = tcr1;
  TimeChangeRule tcr2 = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
  _tcrWintertime = tcr2;
  
  Timezone tz(_tcrSummertime, _tcrWintertime);
  _Tz = tz;
}

time_t EWCTime::now()
{
  return _Tz.toLocal(time(nullptr));
}

EWCTime Time = EWCTime();
