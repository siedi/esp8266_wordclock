#pragma once

#include "JsonListener.h"
#include "EWCConfig.h"

class WeatherListener: public JsonListener {

  public:
    typedef void (*FunctPtr)(int8_t x);
    void weather(FunctPtr callback);
    void temperature(FunctPtr callback);
    
    virtual void whitespace(char c);
  
    virtual void startDocument();

    virtual void key(String key);

    virtual void value(String value);

    virtual void endArray();

    virtual void endObject();

    virtual void endDocument();

    virtual void startArray();

    virtual void startObject();
};

