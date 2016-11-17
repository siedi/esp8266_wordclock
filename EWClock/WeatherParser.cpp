#include "WeatherParser.h"

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
  Serial.println("start document");
  _lastKey = "";
  _temperatureDone = false;
  _weatherDone = false;
}

void WeatherListener::key(String key) {
  Serial.println("key: " + key);
  _lastKey = key;
}

void WeatherListener::value(String val) {
  DEBUG_PRINTLN(val);
  if(_lastKey == "day" && !_temperatureDone) {
    DEBUG_PRINT(F("-temp: "));
    DEBUG_PRINTLN(val);
    
    //_temperature((int8_t)_round_int(val.toFloat()));
    float d = val.toFloat();
    if(d > 0) {
      d = d + 0.5;
    } else {
      d = d - 0.5;
    }
    int8_t x = (int8_t)((d) + ((d) > 0 ? 0.5 : -0.5));
    _temperature((int8_t)d);
    _temperatureDone = true;
  }
  else if (_lastKey == "icon" && !_weatherDone) {
    DEBUG_PRINTLN("-icon: " + val);
    _weatherDone = true;
    if (val == "01d" || val == "01n" ) {
      _weather((int8_t)WEATHER_SONNE);
    }
    else if (val == "02d" || val == "02n") {
      _weather((int8_t)WEATHER_SONNEMITWOLKEN);
    }
    else if (val == "03d" || val == "03n") {
      _weather((int8_t)WEATHER_WOLKEN);
    }
    else if (val == "04d" || val == "04n") {
      _weather((int8_t)WEATHER_WOLKEN);
    }
    else if (val == "09d" || val == "09n") {
      _weather((int8_t)WEATHER_REGEN);
    }
    else if (val == "10d" || val == "10n") {
      _weather((int8_t)WEATHER_REGEN);
    }
    else if (val == "11d" || val == "11n") {
      _weather((int8_t)WEATHER_STURM);
    }
    else if (val == "13d" || val == "13n") {
      _weather((int8_t)WEATHER_SCHNEE);
    }
    else if (val == "50d" || val == "50n") {
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

