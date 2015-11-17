#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <TimeLib.h>
#include <NeoPixelBus.h>
#include <IRremoteESP8266.h>
#include "WebConfig.h"
#include "ewc_def.h"
#include "ewc_telnet.h"
#include "ewc_ntp.h"
#include "ewc_display.h"

WebConfig* pWebConfig;

// IR Remote https://github.com/markszabo/IRremoteESP8266
IRrecv irrecv(IR_RECV_PIN);
decode_results irDecodeResults;
Ticker ldr_timer;
uint8_t command = NOOP;

void checkLDR() {
  if(Display.getAutoBrightness()) {
    DEBUG_PRINT(F("LDR: "));
    int ldrVal = map(analogRead(LDR_PIN), 0, 1023, 0, 100);
    DEBUG_PRINTLN(ldrVal);
    Display.setBrightness(ldrVal);
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

void setup() {

#ifdef DEBUG
  Serial.begin(115200);
#endif

  DEBUG_PRINTLN(F("Starting"));

  DEBUG_PRINTLN(F("Resetting LEDs"));
  Display.begin();

  // Network setup - using a library from the esp8266.com forum for now
  DEBUG_PRINTLN(F("Setting Up Network"));
  pWebConfig = new WebConfig("BASIC WEBCONFIG v1.0", "WORDCLOCK", "123456789 ", false);

  // OTA setup provided by esp8266.com
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECIEVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setHostname("wordclock");
  ArduinoOTA.begin();
  
  // mDNS - go to http://wordclock.local for the network setup
  // with ArduinoOTA we get an MDNSResponder already, so we can skip the MDNS.begin
  MDNS.addService("http", "tcp", 80);

  // Telnet server - no functionality yet
  Telnet.begin();

  // Time Sync and NTP
  if (!NTP.begin()) {
    DEBUG_PRINTLN(F("!!! Error setting up SNTP!"));
  } else {
    DEBUG_PRINTLN(F("Starting Time Sync"));
  }

  // IR setup
  DEBUG_PRINTLN(F("Enabling IR Remote"));
  irrecv.enableIRIn();

  // Initial Display
  command = DISPLAY_CLOCK;

  ldr_timer.attach(10, checkLDR);

  DEBUG_PRINTLN(F("Setup done"));
  DEBUG_PRINT(F("IP address: "));
  DEBUG_PRINTLN(WiFi.localIP());
}

void loop()
{
  pWebConfig->ProcessHTTP();
  ArduinoOTA.handle();
  Telnet.handle();
  
  getIR(command);

  switch (command) {
    case DISPLAY_ONOFF:
      DEBUG_PRINTLN(F("COMMAND: OFF"));
      Display.off();
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
      DEBUG_PRINTLN(F("COMMAND: DISPLAY_CLOCK"));
      Display.setAutoBrightness(true);
      Display.setDisplay(Display.clockLogic, 10);
      break;
    case DISPLAY_FASTTEST:
      DEBUG_PRINTLN(F("COMMAND: FAST TEST"));
      Display.setDisplay(Display.fastTest, 1);
      break;
    case DISPLAY_PROG1:
      DEBUG_PRINTLN(F("COMMAND: makeParty (PROG1)"));
      Display.setDisplay(Display.makeParty, 0.5);
      break;
    case DISPLAY_PROG2:
      DEBUG_PRINTLN(F("COMMAND: showHeart (PROG2)"));
      Display.setDisplay(Display.showHeart, 0.5);
      break;
    case DISPLAY_PROG3:
      DEBUG_PRINTLN(F("COMMAND: fastTest (PROG3)"));
      Display.setDisplay(Display.fastTest, 1);
      break;
    case DISPLAY_PROG4:
      DEBUG_PRINTLN(F("COMMAND: testAll (PROG4)"));
      Display.setDisplay(Display.testAll, 0.08);
      break;
    case NOOP:
      break;
    default:
      DEBUG_PRINT(F("COMMAND UNKNOWN: "));
      DEBUG_PRINTLN(command);
      break;
  }
  command = NOOP;
  yield();
}
