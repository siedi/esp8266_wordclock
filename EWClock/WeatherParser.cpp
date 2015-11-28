#include "WeatherParser.h"
#include "JsonListener.h"
#include "EWCConfig.h"
typedef void (*FunctPtr)(int8_t x);
String _lastKey = "";
FunctPtr _temperature;
FunctPtr _weather;
bool _temperatureDone = false;
bool _weatherDone = false;

void WeatherListener::weather(FunctPtr callback)
{
  _weather = callback;
}

void WeatherListener::temperature(FunctPtr callback)
{
  _temperature = callback;
}

void WeatherListener::whitespace(char c) {
//  Serial.println("whitespace");
}

void WeatherListener::startDocument() {
//  Serial.println("start document");
  _lastKey = "";
  _temperatureDone = false;
  _weatherDone = false;
}

void WeatherListener::key(String key) {
  // Serial.println("key: " + key);
  _lastKey = key;
}

void WeatherListener::value(String value) {
  if (_lastKey == "day" && !_temperatureDone) {
    DEBUG_PRINTLN("-temp: " + value);
    _temperature((int8_t)ROUND_INT(value.toFloat()));
    _temperatureDone = true;
  }
  else if (_lastKey == "icon" && !_weatherDone) {
    DEBUG_PRINTLN("-icon: " + value);
    _weatherDone = true;
    if (value == "01d") {
      _weather((int8_t)WEATHER_SONNE);
    }
    else if (value == "02d") {
      _weather((int8_t)WEATHER_SONNEMITWOLKEN);
    }
    else if (value == "03d") {
      _weather((int8_t)WEATHER_WOLKEN);
    }
    else if (value == "04d") {
      _weather((int8_t)WEATHER_WOLKEN);
    }
    else if (value == "09d") {
      _weather((int8_t)WEATHER_REGEN);
    }
    else if (value == "10d") {
      _weather((int8_t)WEATHER_REGEN);
    }
    else if (value == "11d") {
      _weather((int8_t)WEATHER_STURM);
    }
    else if (value == "13d") {
      _weather((int8_t)WEATHER_SCHNEE);
    }
    else if (value == "50d") {
      _weather((int8_t)WEATHER_NEBEL);
    }
  }
}

void WeatherListener::endArray() {
//  Serial.println("end array. ");
}

void WeatherListener::endObject() {
//  Serial.println("end object. ");
}

void WeatherListener::endDocument() {
//  Serial.println("end document. ");
}

void WeatherListener::startArray() {
//  Serial.println("start array. ");
}

void WeatherListener::startObject() {
//  Serial.println("start object. ");
}
