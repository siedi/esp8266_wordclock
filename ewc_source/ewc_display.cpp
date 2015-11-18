// most of the stuff in here is based on the idea of and borrowed from https://backes-markus.de/blog/2014/02/diy-rgb-strip-wordclock/
#include "ewc_display.h"
#include "ewc_def.h"

int EWCDisplay::_led_strip[NUM_LEDS];
int EWCDisplay::_stackptr = 0;
uint8_t EWCDisplay::_testHours = 0;
uint8_t EWCDisplay::_testMinutes = 0;
#if FEATURE_WEATHER()
int8_t EWCDisplay::_testTemperature = -39;
uint8_t EWCDisplay::_testWeather = 0;
#endif
uint8_t EWCDisplay::_testLED = 0;
uint8_t EWCDisplay::_language_mode = RHEIN_RUHR_MODE;
boolean EWCDisplay::_auto_brightness_enabled = false;
NeoPixelBus EWCDisplay::_led_bus = NeoPixelBus(NUM_LEDS, STRIP_DATA_PIN);
RgbColor EWCDisplay::_leds[NUM_LEDS];
RgbColor EWCDisplay::_red = RgbColor(128, 0, 0);
RgbColor EWCDisplay::_green = RgbColor(0, 128, 0);
RgbColor EWCDisplay::_blue = RgbColor(0, 0, 128);
RgbColor EWCDisplay::_white = RgbColor(128);
RgbColor EWCDisplay::_black = RgbColor(0);
RgbColor EWCDisplay::_default_color = RgbColor(128);
Ticker EWCDisplay::_display_anim;
Ticker EWCDisplay::_led_anim;

EWCDisplay::EWCDisplay() {

}

EWCDisplay::~EWCDisplay(){
  
}

// TODO get rid of and replace with fastled function
RgbColor EWCDisplay::_CHSV(unsigned char h, unsigned char s, unsigned char v) {
  typedef struct rgb
  {
    unsigned char r;
    unsigned char g;
    unsigned char b;
  } rgb;
  typedef struct HsvColor
  {
    unsigned char h;
    unsigned char s;
    unsigned char v;
  } hsv;

  hsv         in;
  in.h = h;
  in.s = s;
  in.v = v;
  double      hh, p, q, t, ff;
  long        i;
  rgb         out;

  if (in.s <= 0.0) {      // < is bogus, just shuts up warnings
    out.r = in.v;
    out.g = in.v;
    out.b = in.v;
    return RgbColor(out.r, out.g, out.b);
  }
  hh = in.h;
  if (hh >= 360.0) hh = 0.0;
  hh /= 60.0;
  i = (long)hh;
  ff = hh - i;
  p = in.v * (1.0 - in.s);
  q = in.v * (1.0 - (in.s * ff));
  t = in.v * (1.0 - (in.s * (1.0 - ff)));

  switch (i) {
    case 0:
      out.r = in.v;
      out.g = t;
      out.b = p;
      break;
    case 1:
      out.r = q;
      out.g = in.v;
      out.b = p;
      break;
    case 2:
      out.r = p;
      out.g = in.v;
      out.b = t;
      break;

    case 3:
      out.r = p;
      out.g = q;
      out.b = in.v;
      break;
    case 4:
      out.r = t;
      out.g = p;
      out.b = in.v;
      break;
    case 5:
    default:
      out.r = in.v;
      out.g = p;
      out.b = q;
      break;
  }
  return RgbColor(out.r, out.g, out.b);
}

void EWCDisplay::_resetStrip() {
  _stackptr = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    _led_strip[i] = 0;
  }
}

void EWCDisplay::_resetAndBlack() {
  _resetStrip();
  for (int i = 0; i < NUM_LEDS; i++) {
    _leds[i] = _black;
  }
}

void EWCDisplay::_showStrip() {
  for (int i = 0; i < NUM_LEDS; i++) {
    _led_bus.SetPixelColor(i, _leds[i]);
  }
  _led_bus.Show();
}

void EWCDisplay::_displayStrip(RgbColor colorCode) {
  for (int i = 0; i < _stackptr; i++) {
    uint8_t j = _led_strip[i];
    _leds[j] = colorCode;
  }
  _showStrip();
}

void EWCDisplay::_displayStrip() {
  _displayStrip(_default_color);
}

void EWCDisplay::_displayStripRandomColor() {
  for (int i = 0; i < _stackptr; i++) {
    _leds[_led_strip[i]] = _CHSV(random(0, 255), 255, 255);
  }
  _showStrip();
}

void EWCDisplay::_pushToStrip(int ledId) {
  _led_strip[_stackptr] = ledId;
  _stackptr++;
}

void EWCDisplay::_pushToStrip(int leds[], uint8_t size) {
  for(int i = 0; i < size; i++) {
    _pushToStrip(leds[i]);
  }
}

void EWCDisplay::_timeToStrip(uint8_t hours, uint8_t minutes) {
  STRIP_PRINT(W1_ES_IST)

  if (minutes >= 5 && minutes < 10) {
    STRIP_PRINT(W1_FUENF1)
    STRIP_PRINT(W1_NACH)
  } else if (minutes >= 10 && minutes < 15) {
    STRIP_PRINT(W1_ZEHN1)
    STRIP_PRINT(W1_NACH)
  } else if (minutes >= 15 && minutes < 20) {
    STRIP_PRINT(W1_VIERTEL)
    STRIP_PRINT(W1_NACH)
  } else if (minutes >= 20 && minutes < 25) {
    if (_language_mode == RHEIN_RUHR_MODE) {
      STRIP_PRINT(W1_ZWANZIG)
      STRIP_PRINT(W1_NACH)
    } else if (_language_mode == WESSI_MODE) {
      STRIP_PRINT(W1_ZEHN1)
      STRIP_PRINT(W1_VOR)
      STRIP_PRINT(W1_HALB)
    }
  } else if (minutes >= 25 && minutes < 30) {
    STRIP_PRINT(W1_FUENF1)
    STRIP_PRINT(W1_VOR)
    STRIP_PRINT(W1_HALB)
  } else if (minutes >= 30 && minutes < 35) {
    STRIP_PRINT(W1_HALB)
  } else if (minutes >= 35 && minutes < 40) {
    STRIP_PRINT(W1_FUENF1)
    STRIP_PRINT(W1_NACH)
    STRIP_PRINT(W1_HALB)
  } else if (minutes >= 40 && minutes < 45) {
    if (_language_mode == RHEIN_RUHR_MODE) {
      STRIP_PRINT(W1_ZWANZIG)
      STRIP_PRINT(W1_VOR)
    } else if (_language_mode == WESSI_MODE) {
      STRIP_PRINT(W1_ZEHN1)
      STRIP_PRINT(W1_NACH)
      STRIP_PRINT(W1_HALB)
    }
  } else if (minutes >= 45 && minutes < 50) {
    STRIP_PRINT(W1_VIERTEL)
    STRIP_PRINT(W1_VOR)
  } else if (minutes >= 50 && minutes < 55) {
    STRIP_PRINT(W1_ZEHN1)
    STRIP_PRINT(W1_VOR)
  } else if (minutes >= 55 && minutes < 60) {
    STRIP_PRINT(W1_FUENF1)
    STRIP_PRINT(W1_VOR)
  }

  int singleMinutes = minutes % 5;
  switch (singleMinutes) {
    case 1:
      STRIP_PRINT(W1_ONE)
      break;
    case 2:
      STRIP_PRINT(W1_ONE)
      STRIP_PRINT(W1_TWO)
      break;
    case 3:
      STRIP_PRINT(W1_ONE)
      STRIP_PRINT(W1_TWO)
      STRIP_PRINT(W1_THREE)
      break;
    case 4:
      STRIP_PRINT(W1_ONE)
      STRIP_PRINT(W1_TWO)
      STRIP_PRINT(W1_THREE)
      STRIP_PRINT(W1_FOUR)
      break;
  }

  if (hours >= 12) {
    hours -= 12;
  }

  if (_language_mode == RHEIN_RUHR_MODE) {
    if (minutes >= 25) {
      hours++;
    }
  } else if (_language_mode == WESSI_MODE) {
    if (minutes >= 20) {
      hours++;
    }
  }

  if (hours == 12) {
    hours = 0;
  }

  //show hours
  switch (hours) {
    case 0:
      STRIP_PRINT(W1_ZWOELF)
      break;
    case 1:
      if (minutes > 4) {
        STRIP_PRINT(W1_EINS)
      } else {
        STRIP_PRINT(W1_EIN)
      }
      break;
    case 2:
      STRIP_PRINT(W1_ZWEI)
      break;
    case 3:
      STRIP_PRINT(W1_DREI)
      break;
    case 4:
      STRIP_PRINT(W1_VIER)
      break;
    case 5:
      STRIP_PRINT(W1_FUENF2)
      break;
    case 6:
      STRIP_PRINT(W1_SECHS)
      break;
    case 7:
      STRIP_PRINT(W1_SIEBEN)
      break;
    case 8:
      STRIP_PRINT(W1_ACHT)
      break;
    case 9:
      STRIP_PRINT(W1_NEUN)
      break;
    case 10:
      STRIP_PRINT(W1_ZEHN)
      break;
    case 11:
      STRIP_PRINT(W1_ELF)
      break;
  }

  //show uhr
  if (minutes < 5) {
    STRIP_PRINT(W1_UHR)
  }
}

#if FEATURE_WEATHER()
void EWCDisplay::_weatherToStrip(int8_t temperature, uint8_t weather) {
  STRIP_PRINT(W2_HEUTE)
  STRIP_PRINT(W2_WIRD)
  STRIP_PRINT(W2_ES)

  if (temperature < 0) {
    STRIP_PRINT(W2_MINUS)
  }

  switch ((int)(abs(temperature) / 5)) {
    case 0:
      STRIP_PRINT(W2_NULL)
      break;
    case 1:
      STRIP_PRINT(W2_FUENF)
      break;   
    case 2:
      STRIP_PRINT(W2_ZEHN)
      break;
    case 3:
      STRIP_PRINT(W2_FUENF)
      STRIP_PRINT(W2_ZEHN)
      break;
    case 4:
      STRIP_PRINT(W2_ZWANZIG)
      break;
    case 5:
      STRIP_PRINT(W2_FUENF)
      STRIP_PRINT(W2_UND)
      STRIP_PRINT(W2_ZWANZIG)
      break;
    case 6:
      STRIP_PRINT(W2_DREISSIG)
      break;
    case 7:
      STRIP_PRINT(W2_FUENF)
      STRIP_PRINT(W2_UND)
      STRIP_PRINT(W2_DREISSIG)
      break;
  }

  switch (abs(temperature) % 5) {
    case 1:
      STRIP_PRINT(W2_ONE)
      break;
    case 2:
      STRIP_PRINT(W2_ONE)
      STRIP_PRINT(W2_TWO)
      break;
    case 3:
      STRIP_PRINT(W2_ONE)
      STRIP_PRINT(W2_TWO)
      STRIP_PRINT(W2_THREE)
      break;
    case 4:
      STRIP_PRINT(W2_ONE)
      STRIP_PRINT(W2_TWO)
      STRIP_PRINT(W2_THREE)
      STRIP_PRINT(W2_FOUR)
      break;
  }

  if (abs(temperature) > 39) {
      STRIP_PRINT(W2_FUENF)
      STRIP_PRINT(W2_UND)
      STRIP_PRINT(W2_DREISSIG)
      STRIP_PRINT(W2_ONE)
      STRIP_PRINT(W2_TWO)
      STRIP_PRINT(W2_THREE)
      STRIP_PRINT(W2_FOUR)
  }

  STRIP_PRINT(W2_GRAD)
  
  if (weather > 0) {
    STRIP_PRINT(W2_UND2)

    switch (weather) {
      case WEATHER_SONNE:
        STRIP_PRINT(W2_SONNE)
        break;
      case WEATHER_NEBEL:
        STRIP_PRINT(W2_NEBEL)
        break;
      case WEATHER_WOLKEN:
        STRIP_PRINT(W2_WOLKEN)
        break;
      case WEATHER_SONNEMITWOLKEN:
        STRIP_PRINT(W2_SONNE)
        STRIP_PRINT(W2_MIT)
        STRIP_PRINT(W2_WOLKEN)
        break;      
      case WEATHER_NIESEL:
        STRIP_PRINT(W2_NIESEL)
        break;
      case WEATHER_SCHNEE:
        STRIP_PRINT(W2_SCHNEE)
        break;       
      case WEATHER_REGEN:
        STRIP_PRINT(W2_REGEN)
        break;
      case WEATHER_SCHNEEREGEN:
        STRIP_PRINT(W2_SCHNEE)
        STRIP_PRINT(W2_REGEN)
        break;
      case WEATHER_STURM:
        STRIP_PRINT(W2_STURM)
        break; 
    }
    STRIP_PRINT(W2_GEBEN)   
  }

}
#endif

void EWCDisplay::_triggerLedAnim() {
  if (_led_bus.IsAnimating())
  {
    _led_bus.UpdateAnimations();
    _led_bus.Show();
  } else {
    _led_anim.detach();
  }
}

void EWCDisplay::_startLedAnim() {
  _led_bus.StartAnimating();
  _led_anim.attach_ms(30, _triggerLedAnim);
}

bool EWCDisplay::begin() {
  
  for (int i = 0; i < NUM_LEDS; i++) {
    _led_strip[i] = 0;
  }
  _led_bus.Begin();
  _resetAndBlack();
  _displayStrip();
  
  return true;
}

void EWCDisplay::setAutoBrightness(boolean enabled) {
  _auto_brightness_enabled = enabled;
}

boolean EWCDisplay::getAutoBrightness() {
  return _auto_brightness_enabled;
}

void EWCDisplay::changeBrightness(int8_t amount) {
  RgbColor prevColor;
  uint8_t speed = 100;
  
  _auto_brightness_enabled = false;
  _display_anim.detach();
  for (int i = 0; i < NUM_LEDS; i++) {
    prevColor = _led_bus.GetPixelColor(i);
    if (amount > 0) {
      prevColor.Lighten(prevColor.CalculateBrightness() / amount);
    } else {
      prevColor.Darken(prevColor.CalculateBrightness() / abs(amount));
    }
    _led_bus.LinearFadePixelColor(speed, i, prevColor);
  }
  _startLedAnim();
}

void EWCDisplay::setBrightness(uint8_t value) {
  RgbColor prevColor;
  uint8_t speed = 100;
  int8_t amount;

  if(value != _old_brightness) {
    amount = value - _old_brightness;
    _old_brightness = value;
    _display_anim.detach();
    for (int i = 0; i < NUM_LEDS; i++) {
      prevColor = _led_bus.GetPixelColor(i);
      if (amount > 0) {
        prevColor.Lighten(prevColor.CalculateBrightness() / amount);
      } else {
        prevColor.Darken(prevColor.CalculateBrightness() / abs(amount));
      }
      _led_bus.LinearFadePixelColor(speed, i, prevColor);
    }
    _startLedAnim();
    
  }

}

void EWCDisplay::off() {
  DEBUG_PRINTLN(F("switching off"));
  _display_anim.detach();
  if (!_led_bus.IsAnimating()) {
    _resetAndBlack();
    for (int i = 0; i < NUM_LEDS; i++) {
      _led_bus.LinearFadePixelColor(1000, i, _black);
    }
    _startLedAnim();
  }
}

void EWCDisplay::showHeart() {
  DEBUG_PRINT(F("HEART..."));
  if (!_led_bus.IsAnimating()) {
    _auto_brightness_enabled = false;
    _resetAndBlack();
    STRIP_PRINT(W1_HEART)
    #if FEATURE_WEATHER()
    STRIP_PRINT(W2_HEART)
    #endif
    _displayStrip(_red);
  }
}

void EWCDisplay::testAll() {
  DEBUG_PRINT(F("SNAKE..."));
  if (!_led_bus.IsAnimating()) {
    _auto_brightness_enabled = false;
    _resetAndBlack();
    _leds[_testLED] = _red;
    if (_testLED > 0) _leds[_testLED - 1] = RgbColor(100, 0, 0);
    if (_testLED > 1) _leds[(_testLED - 2)] = RgbColor(60, 0, 0);
    if (_testLED > 2) _leds[_testLED - 3] = RgbColor(30, 0, 0);
    if (_testLED < NUM_LEDS-1) _leds[_testLED + 1] = RgbColor(100, 0, 0);
    _showStrip();

    _leds[_testLED] = _black;
    if (_testLED > 0) _leds[_testLED - 1] = _black;
    if (_testLED > 1) _leds[(_testLED - 2)] = _black;
    if (_testLED > 2) _leds[_testLED - 3] = _black;
    if (_testLED < NUM_LEDS-1) _leds[_testLED + 1] = _black;

    _testLED++;
    if (_testLED >= NUM_LEDS) {
      _testLED = 0;
    }
  }
}

void EWCDisplay::fastTest() {
  DEBUG_PRINTLN(F("FAST TEST..."));
  if (!_led_bus.IsAnimating()) {
    _auto_brightness_enabled = false;
    if (_testMinutes >= 60) {
      _testMinutes = 0;
      _testHours++;
    }
    if (_testHours >= 24) {
      _testHours = 0;
    }
    if (_testTemperature > 30) {
      _testTemperature = -39;
      _testWeather++;
      if (_testWeather > 9) {
        _testWeather = 0;
      }    
    }
    //Array leeren
    _resetAndBlack();
    _timeToStrip(_testHours, _testMinutes);
    _weatherToStrip(_testTemperature, _testWeather);
    _displayStripRandomColor();
    _testMinutes++;
    _testTemperature++;

  }
}

void EWCDisplay::makeParty() {
  DEBUG_PRINTLN(F("PARTY YEAH..."));
  if (!_led_bus.IsAnimating()) {
    _auto_brightness_enabled = false;
    _resetAndBlack();
    for (int i = 0; i < NUM_LEDS; i++) {
      //leds[i] = CHSV(random(0, 255), 255, 255);
      _leds[i] = RgbColor(random(0, 100), random(0, 100), random(0, 100));
    }
    _showStrip();
  }
}

void EWCDisplay::clockLogic() {
  DEBUG_PRINTLN(F("Clock..."));
  if (!_led_bus.IsAnimating()) {
    _resetAndBlack();
    _timeToStrip(hour(), minute());
    #if FEATURE_WEATHER()
    _weatherToStrip(28, 1);
    #endif
    _displayStrip(_default_color);
  }
}

void EWCDisplay::setDisplay(FunctPtr fp, float seconds) {
  (fp)();
  _display_anim.attach(seconds, fp);
}

EWCDisplay Display = EWCDisplay();
