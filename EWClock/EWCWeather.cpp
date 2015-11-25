#include "EWCWeather.h"
#include "EWCConfig.h"

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
  //const char * path = "/data/2.5/forecast/daily?id=2933799&units=metric&appid=c1c4b462172201936ed0b89097f505e7";
  //const char * host = "api.openweathermap.org";
  //const int httpPort = 80;

  // clear an existing connection first
  if (weatherClient.connected()) {
    Serial.println("flushing old connection");
    weatherClient.flush();
    weatherClient.stop();
  }
  
  if (!weatherClient.connect(OPENWEATHER_HOST, OPENWEATHER_PORT)) {
    Serial.println("connection failed");
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
    Serial.print("Start new sync ");
    Serial.println(millis());
    requestWeather();
    nextSyncTime = millis() + weatherSyncInterval * 1000;
    Serial.print("New Sync time: ");
    Serial.println(nextSyncTime);
    listener.weather(EWCWeather::setWeather);
    listener.temperature(EWCWeather::setTemperature);
    parser.setListener(&listener);
    section="header";
    Serial.println(String(ESP.getFreeHeap()));
  }

  char recieved;
  while (weatherClient.available()){
    //Serial.print(section);
    //Serial.print("-");

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
        //Serial.print(recieved);
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
        // do the json part
        Serial.print(recieved);
        parser.parse(recieved);
      }
    }
    yield();
  }

  if (section == "ignore") {
    // if we are done, we close the connection
    Serial.println("connection no longer needed");
    weatherClient.flush();
    weatherClient.stop();
    section = "done";
    Serial.print("Weather: ");
    Serial.println(_weather);
    Serial.print("Temp: ");
    Serial.println(_temperature);
  }

}


EWCWeather Weather = EWCWeather();
