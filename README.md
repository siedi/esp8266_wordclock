# ESP8266 Wordclock

## Introduction

This is a port of the famous wordclock based on the implementation of [Markus Backes]( https://github.com/bagges/rgb_wordclock) with a couple of extensions. My front panel includes
* a nice image, printed behind acryl glass with the letters blanked out (ask a professional print office, costs around EUR 300 in that size) and
* consists of two wordclocks, the left is as you know it, the right is to show the temperature.

![Wordclock](http://41.media.tumblr.com/c932fcc5221e190e852210302d3322b1/tumblr_nyxwy0TpTF1v135fno7_540.jpg)

[More pictures on Tumblr](http://esp8266wordclock.tumblr.com/)

The design folder includes the SVG (use Inkscape to open it) with the layout of the front panel as well as the light grid and backplane constructed out of wood.

The EWCClock folder contains the source code to build with the Arduino IDE (using the [esp8267.com](https://github.com/esp8266/Arduino) extension).

Have fun.

## Build
1. Download and install the latest Arduino IDE (currently 1.6.6)
2. Download and install the latest [esp8266.com](https://github.com/esp8266/Arduino) source code from their git repository (needed to run with Arduino IDE 1.6.6)
3. Download and Install the following libraries
  * [NeoPixelBus](https://github.com/Makuna/NeoPixelBus), unfortunately the Fastled lib is not available for the ESP8266 yet.
  * [IRremoteESP8266](https://github.com/markszabo/IRremoteESP8266), obviously for the IR remote if you want to use one.
  * [JsonStreamingParser](https://github.com/squix78/json-streaming-parser), parsing the json stream from openweathermap, needs streamer as it is too large to hold completely in memory
4. Clone the repository
5. Rename EWCConfig.h.example to EWCConfig.h and make your changes
6. Compile and upload to your ESP8266 (I'm using the -12 module with 4MB flash)

## TODO
* Fixes:
  * Refactor code, especially display logic (I'm not a C/C++ developer, if someone can help, please PM me)
  * Refactor code
  * Refactor code
  * Fix LDR logic / brightness settings
  * did I say to refactor the code?
* Features:
  * Replace existing webserver with a good interface and include more setting options
  * Replace openweathermap, their forecast and API (with bugs) sucks
  * Implement the snake game (using IR as controller)
  * Attach music controller for disco lights
  * Add the "green" famouse matrix display
  * Add ambilight leds
  * Use feature switches (e.g. one vs. two clocks, IR, LDR)
* Documentation
