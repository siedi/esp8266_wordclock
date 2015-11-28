#ifndef EWC_DISPLAY_H
#define EWC_DISPLAY_H

#include <time.h>
#include <NeoPixelBus.h>
#include <Ticker.h>
#include "EWCConfig.h"
#include "EWCWeather.h"

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
    static int _ledStrip[NUM_LEDS];
    static int _stackptr;
    static uint8_t _testHours;
    static uint8_t _testMinutes;
    #if FEATURE_WEATHER()
    static int8_t _testTemperature;
    static uint8_t _testWeather;
    #endif
    static uint8_t _testLED;
    static uint8_t _languageMode;
    static boolean _autoBrightness;
    static NeoPixelBus _ledBus;
    static RgbColor _leds[NUM_LEDS];
    static RgbColor _red;
    static RgbColor _green;
    static RgbColor _blue;
    static RgbColor _white;
    static RgbColor _black;
    static RgbColor _defaultColor;
    static Ticker _displayAnim;
    static Ticker _ledAnim;
    uint8_t _oldBrightness = 100;

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
    #if FEATURE_WEATHER()
    static void _weatherToStrip(int8_t temperature, int8_t weather);
    #endif
    static void _triggerLedAnim();
    static void _startLedAnim();
};

extern EWCDisplay Display;

#endif

