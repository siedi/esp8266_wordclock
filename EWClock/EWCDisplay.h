#ifndef EWC_DISPLAY_H
#define EWC_DISPLAY_H

#define M_WIDTH 22
#define M_HEIGHT 10

#include <time.h>
#include <NeoPixelBus.h>
#include "EWCConfig.h"
#if FEATURE_WEATHER()
#include "EWCWeather.h"
#endif
#include "ESP8266Console.h"

class EWCDisplay
{
  public:
    EWCDisplay();
    ~EWCDisplay();
    bool begin();
    void handle();
    void setAutoBrightness(boolean enabled);
    boolean getAutoBrightness();
    void setBrightness(uint8_t value);
    void changeBrightness(int8_t amount);
    static void off();
    static void showHeart();
    static void testAll();
    static void fastTest();
    static void makeParty();
    static void fire();
    static void clockLogic(); 
    typedef void (*FunctPtr)();
    void setDisplay(FunctPtr fp);
    void setColor(unsigned char r, unsigned char g, unsigned char b);

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
    static FunctPtr _currentDisplay;
    uint8_t _oldBrightness = 100;
    uint32_t _displayInterval = 1000;
    uint32_t _lastRun = 0;
    static uint8_t _valueMask[M_HEIGHT][M_WIDTH];
    static uint8_t _hueMask[M_HEIGHT][M_WIDTH];
    static uint8_t _matrix[M_WIDTH][M_HEIGHT];
    static uint8_t _line[M_WIDTH];
    static uint8_t _mapping[M_WIDTH][M_HEIGHT];
    static int _pcnt;
    static void _setPixel(unsigned char x, unsigned char y, RgbColor rgb);
    static void _generateLine();
    static void _shiftUp();
    static void _drawFrame(int pcnt);


    static RgbColor _CHSV(unsigned char inh, unsigned char ins, unsigned char inv);
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
};

extern EWCDisplay Display;

#endif
