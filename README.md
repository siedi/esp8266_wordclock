# ESP8266 Wordclok

## Introduction

This is a port of the famous wordclock based on the implementation of [Markus Backes]( https://github.com/bagges/rgb_wordclock) with a couple of extensions. My front panel includes
* a nice image, printed behind acryl glass with the letters blanked out (ask a professional print office, costs around EUR 300 in that size) and
* consists of two wordclocks, the left is as you know it, the right is to show the temperature.

The design folder includes the SVG (use Inkscape to open it) with the layout of the front panel as well as the light grid and backplane constructed out of wood.

The ewc_source folder contains the source code to build with the Arduino IDE (using the [esp8266.com](https://github.com/esp8266/Arduino) extension).

Have fun.

## Build
1. Download and install the latest Arduino IDE (currently 1.6.6)
2. Download and install the latest [esp8266.com](https://github.com/esp8266/Arduino) source code from their git repository (needed to run with Arduino IDE 1.6.6)
3. Download and Install the following libraries
  * [Time](https://www.pjrc.com/teensy/td_libs_Time.html), use the latest from the github (again, needed for the Arduino IDE 1.6.6)
  * [NeoPixelBus](https://github.com/Makuna/NeoPixelBus), unfortunately the Fastled-lib is not available for the ESP8266 yet.
  * [IRremoteESP8266](https://github.com/markszabo/IRremoteESP8266), obviously for the IR remote if you want to use one.
4. Clone the repository and open ewc_source.ino.
5. Compile and upload to your ESP8266 (I'm using the -12 module with the 4MB flash)

## TODO
* Refactor code (I'm not a C/C++ developer, if someone can help, please PM me)
* Implement code for temperature logic (the right wordclock)
* Replace existing webserver with a good interface and include more setting options
* Implement the snake game (use IR as the controller)
* Attach music controller for disco lights
* Add the "green" famouse matrix display
* Seperate ntp library in external repository (for use in other projects as well)
* Documentation
* Use feature switches (e.g. one vs. two clocks, IR, LDR)
