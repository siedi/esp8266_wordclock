#include "EWCConfig.h"
#include "EWCNtp.h"

extern "C" {
 #include "sntp.h"
}

EWCNtp::EWCNtp()
{

}

EWCNtp::~EWCNtp()
{

}

Ticker EWCNtp::TimeCheckTicker;

bool EWCNtp::begin()
{
  sntp_setservername(0, const_cast<char *>(NTP_SERVER));
  sntp_stop();
  sntp_set_timezone(NTP_TIMEZONE);
  sntp_init();
  setSyncInterval(5);
  setSyncProvider(get);
  // start check timer
  // once we have received the time from ntp we want to reduce the sync time
  TimeCheckTicker.attach(10, checkTime);
  return true;
}

time_t EWCNtp::get()
{
  // needs better failure handling
  long ntpTimestamp = sntp_get_current_timestamp();

  DEBUG_PRINT(F("Unix time: "));
  DEBUG_PRINTLN(ntpTimestamp);

  return ntpTimestamp;
}

void ICACHE_FLASH_ATTR EWCNtp::checkTime()
{
  DEBUG_PRINT(F("Run NTP Ckeck..."));
  //if (get == 0) {
  if (timeStatus() == timeNotSet) {
    DEBUG_PRINTLN(F("No time yet - keep on trying!"));
  } else {
    TimeCheckTicker.detach();
    setSyncInterval(60);
    DEBUG_PRINTLN(F("Time set now - check done."));
  }
}

EWCNtp NTP = EWCNtp();
