#ifndef EWC_DEF_H
#define EWC_DEF_H

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

// FEATURES
// #define FEATURE_IR() false
#define FEATURE_WEATHER() true // enables the second wordclock

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
#define W1_ES_IST   L9, L10, L30, L49, L50
#define W1_FUENF1   L70, L89, L90, L109
#define W1_FUENF2   L74, L85, L94, L105
#define W1_NACH     L73, L86, L93, L106
#define W1_ZEHN1    L8, L11, L28, L31
#define W1_VIERTEL  L47, L52, L67, L72, L87, L92, L107
#define W1_VOR      L6, L13, L26
#define W1_HALB     L5, L14, L25, L34
#define W1_ONE      L113
#define W1_TWO      L110
#define W1_THREE    L111
#define W1_FOUR     L112
#define W1_ZWANZIG  L48, L51, L68, L71, L88, L91, L108
#define W1_ZWOELF   L61, L78, L81, L98, L101
#define W1_EIN      L4, L15, L24
#define W1_EINS     W1_EIN, L35
#define W1_ZWEI     L75, L84, L95, L104
#define W1_DREI     L3, L16, L23, L36
#define W1_VIER     L76, L83, L96, L103
#define W1_SECHS    L2, L17, L22, L37, L42
#define W1_SIEBEN   L1, L18, L21, L38, L41, L58
#define W1_ACHT     L77, L82, L97, L102
#define W1_NEUN     L39, L40, L59, L60
#define W1_ZEHN     L0, L19, L20, L39
#define W1_ELF      L54, L65, L74
#define W1_UHR      L80, L99, L100
#define W1_HEART    L29, L30, L70, L89, L11, L48, L68, L91, L7, L52, L107, L6, L106, L5, L105, L15, L95, L23, L83, L37, L77, L41, L61, L59, L32, L46, L45, L44, L43, L55, L56, L57

#if FEATURE_WEATHER()
#define W2_HEUTE    L123, L124, L143, L144, L163
#define W2_WIRD     L183, L184, L203, L204
#define W2_ES       L122, L125
#define W2_GIBT     L145, L162, L165, L182
#define W2_NULL     L185, L202, L205, L222
#define W2_MINUS    L121, L126, L141, L146, L161
#define W2_FUENF    L186, L201, L206, L221
#define W2_UND      L120, L127, L140
#define W2_KUND     L147
#define W2_ZWANZIG  L160, L167, L180, L187, L200, L207, L220
#define W2_DREISSIG L119, L128, L139, L148, L159, L168, L179
#define W2_ZEHN     L188, L199, L208, L219
#define W2_GRAD     L118, L129, L138, L149
#define W2_KUSSMUND L158, L169, L178, L189, L198, L209, L218
#define W2_UND2     L198, L209, L218
#define W2_SONNE    L117, L130, L137, L150, L157
#define W2_NEBEL    L150, L157, L170, L177, L190
#define W2_MIT      L197, L210, L217
#define W2_WOLKEN   L116, L131, L136, L151, L156, L171
#define W2_NIESEL   L171, L176, L191, L196, L211, L216
#define W2_SCHNEE   L115, L132, L135, L152, L155, L172
#define W2_REGEN    L175, L192, L195, L212, L215
#define W2_STURM    L114, L133, L134, L153, L154
#define W2_GEBEN    L174, L193, L194, L213, L214
#define W2_ONE      L227
#define W2_TWO      L224
#define W2_THREE    L225
#define W2_FOUR     L226
#define W2_KUSS     L158, L169, L178
#define W2_HEART    L143, L144, L184, L203, L125, L162, L182, L205, L121, L166, L221, L120, L220, L119, L219, L129, L209, L137, L197, L151, L191, L155, L175, L173, W2_KUSS

// the weather conditions
#define WEATHER_UNKNOWN 0
#define WEATHER_SONNE 1
#define WEATHER_NEBEL 2
#define WEATHER_WOLKEN 3
#define WEATHER_SONNEMITWOLKEN 4
#define WEATHER_NIESEL 5
#define WEATHER_SCHNEE 6
#define WEATHER_REGEN 7
#define WEATHER_SCHNEEREGEN 8
#define WEATHER_STURM 9
#endif

#define STRIP_PRINT(braces) { int array[] = {braces}; _pushToStrip(array, COUNT_OF(array)); }
#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))

#endif // EWC_DEF_H
