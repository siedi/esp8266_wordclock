#ifndef DEF_H
#define DEF_H

#include "Arduino.h"

#include "ewc_layout.h"

// Enable DEBUG messages
#define DEBUG

// LEDs
#define NUM_LEDS 228

// PINs
#define STRIP_DATA_PIN 5  // LED strip PIN
#define IR_RECV_PIN 4     // IR remote PIN
#define LDR_PIN A0        // LDR PIN

// TIME
#define NTP_TIMEZONE 1
#define NTP_SERVER "time.nist.gov"

// TELNET
#define TELNET_MAX 1
#define TELNET_PORT 23

// FEATURES
#define FEATURE_IR() false

// COMMANDS
#define NOOP 0
#define DISPLAY_ONOFF 1
#define BRIGHTNESS_AUTO 2
#define BRIGHTNESS_DIM 3
#define BRIGHTNESS_RAISE 4
#define DISPLAY_CLOCK 5
#define DISPLAY_FASTTEST 6
#define DISPLAY_PROG1 7
#define DISPLAY_PROG2 8
#define DISPLAY_PROG3 9
#define DISPLAY_PROG4 10

// IR MODE defines
#define IR_ONOFF 0xFF00FF
#define IR_DIM 0xFFD02F
#define IR_RAISE 0xFF50AF
#define IR_CLOCK 0xFF30CF
#define IR_PROG1 0xFFB04F
#define IR_PROG2 0xFF08F7
#define IR_PROG3 0xFF708F
#define IR_PROG4 0xFFF00F


#define RHEIN_RUHR_MODE 0
#define WESSI_MODE 1

#ifdef DEBUG
  #define DEBUG_PRINT(str)  Serial.print (str)
  #define DEBUG_PRINTLN(str)  Serial.println (str)
#else
  #define DEBUG_PRINT(str)
  #define DEBUG_PRINTLN(str)
#endif

#endif // DEF_H
