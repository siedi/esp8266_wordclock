#ifndef EWC_TELNET_H
#define EWC_TELNET_H

//#include <inttypes.h>
#include <ESP8266WiFi.h>

#include "Stream.h"
#include "EWCConfig.h"

enum TLogLevel {logERROR, logWARNING, logINFO, logDEBUG};

class ESP8266Console : public Stream
{
  public:

    ESP8266Console();
    ~ESP8266Console();
    void begin();
    void handle();
    
    size_t write(uint8_t) override;
    size_t write(const uint8_t *, size_t) override;

    int available(void) override;
    int read(void) override;
    int peek(void) override;
    void flush(void) override;

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write;

  private:
    uint8_t _logLevel = 0;
};

extern ESP8266Console Console;

#endif

