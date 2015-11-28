#ifndef EWC_TELNET_H
#define EWC_TELNET_H

#include <ESP8266WiFi.h>

class EWCTelnet
{
  public:

    EWCTelnet();
    ~EWCTelnet();
    bool begin();
    void handle();
    void print(const char* str);
    void println(const char* str);
    
  private:
};

extern EWCTelnet Telnet;

#endif


