#ifndef DEF_H
#define DEF_H

#include "Arduino.h"
#include "ewc_layout.h"

// Enable DEBUG messages
#define DEBUG

// LEDs - total amount 114 for single wordlock, 228 for dual
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

// FEATURES -- not implemented yet
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

// Usually doesn't need any changes below
#define RHEIN_RUHR_MODE 0
#define WESSI_MODE 1

#ifdef DEBUG
  #define DEBUG_PRINT(str)  Serial.print (str)
  #define DEBUG_PRINTLN(str)  Serial.println (str)
#else
  #define DEBUG_PRINT(str)
  #define DEBUG_PRINTLN(str)
#endif

// The Words
#define W_ES_IST  L9, L10, L30, L49, L50
#define W_FUENF1  L70, L89, L90, L109
#define W_FUENF2  L74, L85, L94, L105
#define W_NACH    L73, L86, L93, L106
#define W_ZEHN1   L8, L11, L28, L31
#define W_VIERTEL L47, L52, L67, L72, L87, L92, L107
#define W_VOR     L6, L13, L26
#define W_HALB    L5, L14, L25, L34
#define W_ONE     L113
#define W_TWO     L110
#define W_THREE   L111
#define W_FOUR    L112
#define W_ZWANZIG L48, L51, L68, L71, L88, L91, L108
#define W_ZWOELF  L61, L78, L81, L98, L101
#define W_EIN     L4, L15, L24
#define W_EINS    W_EIN, L35
#define W_ZWEI    L75, L84, L95, L104
#define W_DREI    L3, L16, L23, L36
#define W_VIER    L76, L83, L96, L103
#define W_SECHS   L2, L17, L22, L37, L42
#define W_SIEBEN  L1, L18, L21, L38, L41, L58
#define W_ACHT    L77, L82, L97, L102
#define W_NEUN    L39, L40, L59, L60
#define W_ZEHN    L0, L19, L20, L39
#define W_ELF     L54, L65, L74
#define W_UHR     L80, L99, L100

#define STRIP_PRINT(braces) { int array[] = {braces}; _pushToStrip(array, COUNT_OF(array)); }
#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))

#endif // DEF_H
