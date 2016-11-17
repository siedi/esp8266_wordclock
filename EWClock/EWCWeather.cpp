#include "EWCWeather.h"

int8_t EWCWeather::_temperature = 0;
int8_t EWCWeather::_weather = WEATHER_UNKNOWN;
uint32_t EWCWeather::weatherSyncInterval = 30;
uint32_t EWCWeather::nextSyncTime = 0;
EWCWeather::weatherStatus_t weatherStatus = EWCWeather::weatherNotSet;
    
void EWCWeather::setWeather(int8_t weather) {
  _weather = weather;
}

void EWCWeather::setTemperature(int8_t temperature) {
  _temperature = temperature;
}

int8_t EWCWeather::getWeather() {
  return _weather;
}

int8_t EWCWeather::getTemperature() {
  return _temperature;
}

void EWCWeather::requestWeather()
{
  // clear any existing connection first
  if (weatherClient.connected()) {
    weatherClient.flush();
    weatherClient.stop();
  }
  
  if (!weatherClient.connect(OPENWEATHER_HOST, OPENWEATHER_PORT)) {
    DEBUG_PRINTLN(F("Weather connection failed"));
    return;
  }

  weatherClient.print(String("GET ") + OPENWEATHER_URL + " HTTP/1.1\r\n" +
               "Host: " + OPENWEATHER_HOST + "\r\n" + 
               "Connection: keep-alive\r\n\r\n");
}

void EWCWeather::checkWeather()
{
  // check if we need a new connection and sync time expired, then start request
  // we only have to check for the syn-periode as the client still might be connected, but we dont care.
 
  if (millis() > nextSyncTime) {
    DEBUG_PRINTLN("Weather start new sync");
    requestWeather();
    nextSyncTime = millis() + weatherSyncInterval * 1000;
    listener.weather(EWCWeather::setWeather);
    listener.temperature(EWCWeather::setTemperature);
    parser.setListener(&listener);
    section="header";
  }

  char recieved;
  while (weatherClient.available()){
    recieved = weatherClient.read();
    if (section == "header") {
      if (recieved == '\n') {
        section = "newline";
      }
    }
    else if (section == "newline") {
      if (recieved == '\n') {
        section = "bodynewline";      
      }
      else if (recieved == '\r') {
        // ignore this char
      } else {
        section = "header";
      }
    }
    else if (section == "bodynewline") {
      if (recieved == '{') {
        section = "jsonstart";
        parser.parse(recieved);
      }
      else if (recieved == '\n') {
        section = "bodynewline";
      }
      else {
        section = "body";
      }
    }
    else if (section == "body") {
      if (recieved == '\n') {
        section = "bodynewline";
      }
    }
    else if (section == "jsonstart") {
      if (recieved == '\n') {
        section = "ignore"; // we just read one line form the body
      } else {
        parser.parse(recieved);
      }
    }
    yield();
  }

  if (section == "ignore") {
    weatherClient.flush();
    weatherClient.stop();
    section = "done";
    DEBUG_PRINT(F("Weather: "));
    DEBUG_PRINTLN(_weather);
    DEBUG_PRINT(F("Temp: "));
    DEBUG_PRINTLN(_temperature);
  }
}

EWCWeather Weather = EWCWeather();
