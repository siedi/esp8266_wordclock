#ifndef EWC_WEATHER_H
#define EWC_WEATHER_H

#include <ESP8266WiFi.h>
#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include "WeatherParser.h"

class EWCWeather
{
  public:
    static void setWeather(int8_t weather);
    static void setTemperature(int8_t temperature);
    static int8_t getWeather();
    static int8_t getTemperature();
    void requestWeather();
    void checkWeather();
    typedef enum {
      weatherNotSet, 
      weatherNeedsSync, 
      weatherSet
    }  weatherStatus_t ;
        
  private:
    // https://github.com/squix78/json-streaming-parser
    JsonStreamingParser parser;
    WeatherListener listener;
    WiFiClient weatherClient;

    static int8_t _temperature;
    static int8_t _weather;
    static uint32_t weatherSyncInterval;
    static uint32_t nextSyncTime;
    static weatherStatus_t weatherStatus;
    String section;
};

extern EWCWeather Weather;

#endif

