#ifndef EWC_DISPLAY_H
#define EWC_DISPLAY_H

#include "ewc_def.h"
#include <NeoPixelBus.h>
#include <TimeLib.h>
#include <Ticker.h>

class EWCDisplay
{
  public:

    EWCDisplay();
    ~EWCDisplay();
    bool begin();
    void setAutoBrightness(boolean enabled);
    boolean getAutoBrightness();
    void setBrightness(uint8_t value);
    void changeBrightness(int8_t amount);
    static void off();
    static void showHeart();
    static void testAll();
    static void fastTest();
    static void makeParty();
    static void clockLogic(); 
    typedef void (*FunctPtr)();
    void setDisplay(FunctPtr fp, float seconds);

  private:
    static int _led_strip[NUM_LEDS];
    static int _stackptr;
    static uint8_t _testHours;
    static uint8_t _testMinutes;
    static uint8_t _testLED;
    static uint8_t _language_mode;
    static boolean _auto_brightness_enabled;
    static NeoPixelBus _led_bus;
    static RgbColor _leds[NUM_LEDS];
    static RgbColor _red;
    static RgbColor _green;
    static RgbColor _blue;
    static RgbColor _white;
    static RgbColor _black;
    static RgbColor _default_color;
    static Ticker _display_anim;
    static Ticker _led_anim;
    uint8_t _old_brightness = 100;

    static RgbColor _CHSV(unsigned char h, unsigned char s, unsigned char v);
    static void _resetAndBlack();
    static void _resetStrip();
    static void _showStrip();
    static void _displayStrip();
    static void _displayStrip(RgbColor colorCode);
    static void _displayStripRandomColor() ;
    static void _pushToStrip(int ledId);
    static void _pushToStrip(int leds[], uint8_t size);
    static void _timeToStrip(uint8_t hours, uint8_t minutes);
    static void _triggerLedAnim();
    static void _startLedAnim();
};

extern EWCDisplay Display;

#endif
