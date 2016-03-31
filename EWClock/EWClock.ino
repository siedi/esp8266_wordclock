#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <NeoPixelBus.h>
#include <IRremoteESP8266.h>
#include "WebConfig.h"
#include "EWCConfig.h"
#include "ESP8266Console.h"
#include "EWCTime.h"
#include "EWCDisplay.h"
#if FEATURE_WEATHER()
#include "EWCWeather.h"
#endif
#include "EWCWebserver.h"
#include "FS.h"

WebConfig* pWebConfig;

// IR Remote https://github.com/markszabo/IRremoteESP8266
IRrecv irrecv(IR_RECV_PIN);
decode_results irDecodeResults;
Ticker ldrTimer;

uint8_t command = NOOP;

void checkLDR()
{
  if (Display.getAutoBrightness()) {
    DEBUG_PRINT(F("LDR: "));
    int ldrVal = map(analogRead(LDR_PIN), 0, 1023, 0, 100);
    DEBUG_PRINTLN(ldrVal);
    // Display.setBrightness(ldrVal);
  }
}

boolean getIR(uint8_t &command) {
  if (irrecv.decode(&irDecodeResults)) {
    DEBUG_PRINTLN(F("Received IR code"));
    delay(50);
    switch (irDecodeResults.value) {
      case IR_ONOFF:
        command = DISPLAY_ONOFF;
        break;
      case IR_RAISE:
        command = BRIGHTNESS_RAISE;
        break;
      case IR_DIM:
        command = BRIGHTNESS_DIM;
        break;
      case IR_PROG1:
        command = DISPLAY_PROG1;
        break;
      case IR_PROG2:
        command = DISPLAY_PROG2;
        break;
      case IR_PROG3:
        command = DISPLAY_PROG3;
        break;
      case IR_PROG4:
        command = DISPLAY_PROG4;
        break;
      case IR_CLOCK:
        command = DISPLAY_CLOCK;
        break;
      case IR_RED:
        command = COLOR_RED;
        break;
      case IR_GREEN:
        command = COLOR_GREEN;
        break;
      case IR_PURPLE:
        command = COLOR_PURPLE;
        break;
      case IR_YELLOW:
        command = COLOR_YELLOW;
        break;
      case IR_LIGHTBLUE:
        command = COLOR_LIGHTBLUE;
        break;
      case IR_VIOLET:
        command = COLOR_VIOLET;
        break;
      case IR_ORANGE:
        command = COLOR_ORANGE;
        break;
      case IR_BLUE:
        command = COLOR_BLUE;
        break;
      case IR_MAGENTA:
        command = COLOR_MAGENTA;
        break;
      default:
        DEBUG_PRINT(F("IR UNKNOWN: "));
        DEBUG_PRINTLN(String(irDecodeResults.value, HEX));
        break;
    }
    irrecv.resume();
    return true;
  }
  return false;
}

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  Serial.setDebugOutput(true);
#endif

  DEBUG_PRINTLN(F("Starting"));
  DEBUG_PRINTLN("Mounting FS...");

  if (!SPIFFS.begin()) {
    DEBUG_PRINTLN("Failed to mount file system");
    return;
  }

  DEBUG_PRINTLN(F("Resetting LEDs"));
  Display.begin();

  // Network setup - using a library from the esp8266.com forum for now
  DEBUG_PRINTLN(F("Setting Up Network"));
  pWebConfig = new WebConfig("BASIC WEBCONFIG v1.0", "wordclock", "12345678901234567890 ", false);

  // OTA setup provided by esp8266.com
  ArduinoOTA.setHostname("wordclock");

  ArduinoOTA.onStart([]() {
    Console.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Console.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Console.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Console.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Console.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Console.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Console.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Console.println("Receive Failed");
    else if (error == OTA_END_ERROR) Console.println("End Failed");
  });
  ArduinoOTA.begin();

  // mDNS - go to http://wordclock.local for the network setup
  // with ArduinoOTA we get an MDNSResponder already, so we can skip the MDNS.begin
  MDNS.addService("http", "tcp", 80);

  Console.begin();

  DEBUG_PRINTLN(F("Setting up SNTP")); 
  Time.begin();

  DEBUG_PRINTLN(F("Enabling IR Remote"));
  irrecv.enableIRIn();

  DEBUG_PRINTLN(F("Enabling IR Remote"));
  ldrTimer.attach(10, checkLDR);

  DEBUG_PRINTLN(F("Setup Webserver"));
  Webserver.begin(81, Display);

  command = DISPLAY_CLOCK;

  DEBUG_PRINTLN(F("Setup done"));
  DEBUG_PRINT(F("IP address: "));
  DEBUG_PRINTLN(WiFi.localIP());
}

void loop()
{
  pWebConfig->ProcessHTTP();
  Console.handle();
  ArduinoOTA.handle();
  Weather.checkWeather();
  Webserver.handle();
  getIR(command);

  switch (command) {
    case DISPLAY_ONOFF:
      DEBUG_PRINTLN(F("COMMAND: OFF"));
      Display.setDisplay(Display.off);
      break;
    case BRIGHTNESS_RAISE:
      DEBUG_PRINTLN(F("COMMAND: BRIGHTNESS_RAISE"));
      Display.changeBrightness(2);
      break;
    case BRIGHTNESS_DIM:
      DEBUG_PRINTLN(F("COMMAND: BRIGHTNESS_DIM"));
      Display.changeBrightness(-2);
      break;
    case DISPLAY_CLOCK:
      DEBUG_PRINTLN("COMMAND: DISPLAY_CLOCK");
      Display.setAutoBrightness(true);
      Display.setDisplay(Display.clockLogic);
      break;
    case DISPLAY_FASTTEST:
      DEBUG_PRINTLN(F("COMMAND: FAST TEST"));
      Display.setDisplay(Display.fastTest);
      break;
    case DISPLAY_PROG1:
      DEBUG_PRINTLN(F("COMMAND: makeParty (PROG1)"));
      Display.setDisplay(Display.makeParty);
      break;
    case DISPLAY_PROG2:
      DEBUG_PRINTLN(F("COMMAND: showHeart (PROG2)"));
      Display.setDisplay(Display.showHeart);
      break;
    case DISPLAY_PROG3:
      DEBUG_PRINTLN(F("COMMAND: fastTest (PROG3)"));
      Display.setDisplay(Display.fire);
      break;
    case DISPLAY_PROG4:
      DEBUG_PRINTLN(F("COMMAND: testAll (PROG4)"));
      Display.setDisplay(Display.testAll);
      break;
    case COLOR_RED:
      DEBUG_PRINTLN(F("COMMAND: color red"));
      Display.setColor(128, 0, 0);
      break;
    case COLOR_GREEN:
      DEBUG_PRINTLN(F("COMMAND: color green"));
      Display.setColor(0, 128, 0);
      break;
    case COLOR_PURPLE:
      DEBUG_PRINTLN(F("COMMAND: color purple"));
      Display.setColor(128, 0, 128);
      break;
    case COLOR_YELLOW:
      DEBUG_PRINTLN(F("COMMAND: color yellow"));
      Display.setColor(128, 128, 0);
      break;
    case COLOR_LIGHTBLUE:
      DEBUG_PRINTLN(F("COMMAND: color lightblue"));
      Display.setColor(0, 64, 128);
      break;
    case COLOR_VIOLET:
      DEBUG_PRINTLN(F("COMMAND: color violet"));
      Display.setColor(128, 64, 128);
      break;
      break;
    case COLOR_ORANGE:
      DEBUG_PRINTLN(F("COMMAND: color violet"));
      Display.setColor(128, 64, 0);
      break;
    case COLOR_BLUE:
      DEBUG_PRINTLN(F("COMMAND: color blue"));
      Display.setColor(0, 0, 128);
      break;
    case COLOR_MAGENTA:
      DEBUG_PRINTLN(F("COMMAND: color magenta"));
      Display.setColor(64, 0, 128);
      break;
    case NOOP:
      break;
    default:
      DEBUG_PRINT(F("COMMAND UNKNOWN: "));
      DEBUG_PRINTLN(command);
      break;
  }
  command = NOOP;
  Display.handle();
  yield();
}
