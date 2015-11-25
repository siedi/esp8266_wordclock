#ifndef EWC_NTP_H
#define EWC_NTP_H

#include <TimeLib.h>
#include <Ticker.h>

class EWCNtp
{
  public:
    EWCNtp();
    ~EWCNtp();
    bool begin();
    static time_t get();
    static void checkTime();
    static Ticker TimeCheckTicker;   
  private:
};

extern EWCNtp NTP;

#endif
