// most of the stuff in here is based the idea and borrowed from https://backes-markus.de/blog/2014/02/diy-rgb-strip-wordclock/
#include "ewc_display.h"
#include "ewc_def.h"
// #include <functional>

int EWCDisplay::_led_strip[NUM_LEDS];
int EWCDisplay::_stackptr = 0;
uint8_t EWCDisplay::_testHours = 0;
uint8_t EWCDisplay::_testMinutes = 0;
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

///////////////////////
//PUSH WORD HELPER
///////////////////////
void EWCDisplay::_pushES_IST()  {
  _pushToStrip(L9);
  _pushToStrip(L10);
  _pushToStrip(L30);
  _pushToStrip(L49);
  _pushToStrip(L50);
}

void EWCDisplay::_pushFUENF1() {
  _pushToStrip(L70);
  _pushToStrip(L89);
  _pushToStrip(L90);
  _pushToStrip(L109);
}

void EWCDisplay::_pushFUENF2() {
  _pushToStrip(L74);
  _pushToStrip(L85);
  _pushToStrip(L94);
  _pushToStrip(L105);
}

void EWCDisplay::_pushNACH() {
  _pushToStrip(L73);
  _pushToStrip(L86);
  _pushToStrip(L93);
  _pushToStrip(L106);
}

void EWCDisplay::_pushZEHN1() {
  _pushToStrip(L8);
  _pushToStrip(L11);
  _pushToStrip(L28);
  _pushToStrip(L31);
}

void EWCDisplay::_pushVIERTEL() {
  _pushToStrip(L47);
  _pushToStrip(L52);
  _pushToStrip(L67);
  _pushToStrip(L72);
  _pushToStrip(L87);
  _pushToStrip(L92);
  _pushToStrip(L107);
}

void EWCDisplay::_pushVOR() {
  _pushToStrip(L6);
  _pushToStrip(L13);
  _pushToStrip(L26);
}

void EWCDisplay::_pushHALB() {
  _pushToStrip(L5);
  _pushToStrip(L14);
  _pushToStrip(L25);
  _pushToStrip(L34);
}

void EWCDisplay::_pushONE() {
  _pushToStrip(L113);
}

void EWCDisplay::_pushTWO() {
  _pushToStrip(L110);
}

void EWCDisplay::_pushTHREE() {
  _pushToStrip(L111);
}

void EWCDisplay::_pushFOUR() {
  _pushToStrip(L112);
}

void EWCDisplay::_pushZWANZIG() {
  _pushToStrip(L48);
  _pushToStrip(L51);
  _pushToStrip(L68);
  _pushToStrip(L71);
  _pushToStrip(L88);
  _pushToStrip(L91);
  _pushToStrip(L108);
}

void EWCDisplay::_pushZWOELF() {
  _pushToStrip(L61);
  _pushToStrip(L78);
  _pushToStrip(L81);
  _pushToStrip(L98);
  _pushToStrip(L101);
}

void EWCDisplay::_pushEINS(bool s) {
  _pushToStrip(L4);
  _pushToStrip(L15);
  _pushToStrip(L24);
  if (s) {
    _pushToStrip(L35);
  }
}

void EWCDisplay::_pushZWEI() {
  _pushToStrip(L75);
  _pushToStrip(L84);
  _pushToStrip(L95);
  _pushToStrip(L104);
}

void EWCDisplay::_pushDREI() {
  _pushToStrip(L3);
  _pushToStrip(L16);
  _pushToStrip(L23);
  _pushToStrip(L36);
}

void EWCDisplay::_pushVIER() {
  _pushToStrip(L76);
  _pushToStrip(L83);
  _pushToStrip(L96);
  _pushToStrip(L103);
}

void EWCDisplay::_pushSECHS() {
  _pushToStrip(L2);
  _pushToStrip(L17);
  _pushToStrip(L22);
  _pushToStrip(L37);
  _pushToStrip(L42);
}

void EWCDisplay::_pushSIEBEN() {
  _pushToStrip(L1);
  _pushToStrip(L18);
  _pushToStrip(L21);
  _pushToStrip(L38);
  _pushToStrip(L41);
  _pushToStrip(L58);
}

void EWCDisplay::_pushACHT() {
  _pushToStrip(L77);
  _pushToStrip(L82);
  _pushToStrip(L97);
  _pushToStrip(L102);
}

void EWCDisplay::_pushNEUN() {
  _pushToStrip(L39);
  _pushToStrip(L40);
  _pushToStrip(L59);
  _pushToStrip(L60);
}

void EWCDisplay::_pushZEHN() {
  _pushToStrip(L0);
  _pushToStrip(L19);
  _pushToStrip(L20);
  _pushToStrip(L39);
}

void EWCDisplay::_pushELF() {
  _pushToStrip(L54);
  _pushToStrip(L65);
  _pushToStrip(L74);
}

void EWCDisplay::_pushUHR() {
  _pushToStrip(L80);
  _pushToStrip(L99);
  _pushToStrip(L100);
}
///////////////////////

void EWCDisplay::_timeToStrip(uint8_t hours, uint8_t minutes) {
  _pushES_IST();

  //show minutes
  if (minutes >= 5 && minutes < 10) {
    _pushFUENF1();
    _pushNACH();
  } else if (minutes >= 10 && minutes < 15) {
    _pushZEHN1();
    _pushNACH();
  } else if (minutes >= 15 && minutes < 20) {
    _pushVIERTEL();
    _pushNACH();
  } else if (minutes >= 20 && minutes < 25) {
    if (_language_mode == RHEIN_RUHR_MODE) {
      _pushZWANZIG();
      _pushNACH();
    } else if (_language_mode == WESSI_MODE) {
      _pushZEHN1();
      _pushVOR();
      _pushHALB();
    }
  } else if (minutes >= 25 && minutes < 30) {
    _pushFUENF1();
    _pushVOR();
    _pushHALB();
  } else if (minutes >= 30 && minutes < 35) {
    _pushHALB();
  } else if (minutes >= 35 && minutes < 40) {
    _pushFUENF1();
    _pushNACH();
    _pushHALB();
  } else if (minutes >= 40 && minutes < 45) {
    if (_language_mode == RHEIN_RUHR_MODE) {
      _pushZWANZIG();
      _pushVOR();
    } else if (_language_mode == WESSI_MODE) {
      _pushZEHN1();
      _pushNACH();
      _pushHALB();
    }
  } else if (minutes >= 45 && minutes < 50) {
    _pushVIERTEL();
    _pushVOR();
  } else if (minutes >= 50 && minutes < 55) {
    _pushZEHN1();
    _pushVOR();
  } else if (minutes >= 55 && minutes < 60) {
    _pushFUENF1();
    _pushVOR();
  }

  int singleMinutes = minutes % 5;
  switch (singleMinutes) {
    case 1:
      _pushONE();
      break;
    case 2:
      _pushONE();
      _pushTWO();
      break;
    case 3:
      _pushONE();
      _pushTWO();
      _pushTHREE();
      break;
    case 4:
      _pushONE();
      _pushTWO();
      _pushTHREE();
      _pushFOUR();
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
      _pushZWOELF();
      break;
    case 1:
      if (minutes > 4) {
        _pushEINS(true);
      } else {
        _pushEINS(false);
      }
      break;
    case 2:
      _pushZWEI();
      break;
    case 3:
      _pushDREI();
      break;
    case 4:
      _pushVIER();
      break;
    case 5:
      _pushFUENF2();
      break;
    case 6:
      _pushSECHS();
      break;
    case 7:
      _pushSIEBEN();
      break;
    case 8:
      _pushACHT();
      break;
    case 9:
      _pushNEUN();
      break;
    case 10:
      _pushZEHN();
      break;
    case 11:
      _pushELF();
      break;
  }

  //show uhr
  if (minutes < 5) {
    _pushUHR();
  }
}

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
    _pushToStrip(L29);  _pushToStrip(L30);  _pushToStrip(L70);  _pushToStrip(L89);
    _pushToStrip(L11);  _pushToStrip(L48);  _pushToStrip(L68);  _pushToStrip(L91);
    _pushToStrip(L7);   _pushToStrip(L52);  _pushToStrip(L107);
    _pushToStrip(L6);   _pushToStrip(L106);
    _pushToStrip(L5);   _pushToStrip(L105);
    _pushToStrip(L15);  _pushToStrip(L95);
    _pushToStrip(L23);  _pushToStrip(L83);
    _pushToStrip(L37);  _pushToStrip(L77);
    _pushToStrip(L41);  _pushToStrip(L61);
    _pushToStrip(L59);
    // text
    _pushToStrip(L32);
    _pushToStrip(L46);  _pushToStrip(L45);  _pushToStrip(L44);  _pushToStrip(L43);
    _pushToStrip(L55);  _pushToStrip(L56);  _pushToStrip(L57);
    
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
    if (_testLED < NUM_LEDS) _leds[_testLED + 1] = RgbColor(100, 0, 0);
    _showStrip();

    _leds[_testLED] = _black;
    if (_testLED > 0) _leds[_testLED - 1] = _black;
    if (_testLED > 1) _leds[(_testLED - 2)] = _black;
    if (_testLED > 2) _leds[_testLED - 3] = _black;
    if (_testLED < NUM_LEDS) _leds[_testLED + 1] = _black;

    _testLED++;
    if (_testLED >= NUM_LEDS) {
      _testLED = 0;
    }
  }
}

void EWCDisplay::fastTest() {
  DEBUG_PRINT(F("FAST TEST..."));
  if (!_led_bus.IsAnimating()) {
    _auto_brightness_enabled = false;
    if (_testMinutes >= 60) {
      _testMinutes = 0;
      _testHours++;
    }
    if (_testHours >= 24) {
      _testHours = 0;
    }

    //Array leeren
    _resetAndBlack();
    _timeToStrip(_testHours, _testMinutes);
    _displayStripRandomColor();
    _testMinutes++;
  }
}

void EWCDisplay::makeParty() {
  DEBUG_PRINT(F("PARTY YEAH..."));
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
    _displayStrip(_default_color);
  }
}

void EWCDisplay::setDisplay(FunctPtr fp, float seconds) {
  (fp)();
  _display_anim.attach(seconds, fp);
}

EWCDisplay Display = EWCDisplay();
