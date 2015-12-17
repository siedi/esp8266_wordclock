// most of the stuff in here is based on the idea of and borrowed from https://backes-markus.de/blog/2014/02/diy-rgb-strip-wordclock/
#include "EWCDisplay.h"

int EWCDisplay::_ledStrip[NUM_LEDS];
int EWCDisplay::_stackptr = 0;
uint8_t EWCDisplay::_testHours = 0;
uint8_t EWCDisplay::_testMinutes = 0;
#if FEATURE_WEATHER()
int8_t EWCDisplay::_testTemperature = -39;
uint8_t EWCDisplay::_testWeather = 0;
#endif
uint8_t EWCDisplay::_testLED = 0;
uint8_t EWCDisplay::_languageMode = RHEIN_RUHR_MODE;
boolean EWCDisplay::_autoBrightness = false;
NeoPixelBus EWCDisplay::_ledBus = NeoPixelBus(NUM_LEDS, STRIP_DATA_PIN);
RgbColor EWCDisplay::_leds[NUM_LEDS];
RgbColor EWCDisplay::_red = RgbColor(128, 0, 0);
RgbColor EWCDisplay::_green = RgbColor(0, 128, 0);
RgbColor EWCDisplay::_blue = RgbColor(0, 0, 128);
RgbColor EWCDisplay::_white = RgbColor(128);
RgbColor EWCDisplay::_black = RgbColor(0);
RgbColor EWCDisplay::_defaultColor = _white;
typedef void (*FunctPtr)();
FunctPtr EWCDisplay::_currentDisplay;
const char* EWCDisplay::_displayList[] = {"off", "heart", "test", "fasttest", "party", "fire", "clock", 0};

// for the fire from https://github.com/giladaya/arduino-led-matrix/blob/master/fire/fire.ino
//these values are substracetd from the generated values to give a shape to the animation
uint8_t EWCDisplay::_valueMask[M_HEIGHT][M_WIDTH] = {
    {32 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
    {48 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 48 , 48 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 48 },
    {64 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 48 , 48 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
    {96 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 64 , 64 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 96 },
    {128, 64 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 96 , 96 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 64 , 128},
    {160, 96 , 64 , 32 , 0  , 0  , 0  , 0  , 32 , 64 , 128, 128, 64 , 32 , 0  , 0  , 0  , 0  , 32 , 64 , 96 , 160},
    {192, 128, 96 , 64 , 32 , 0  , 0  , 32 , 64 , 96 , 160, 160, 96 , 64 , 32 , 0  , 0  , 32 , 64 , 96 , 128, 192},
    {255, 160, 128, 96 , 96 , 64 , 32 , 64 , 96 , 128, 192, 192, 128, 96 , 64 , 32 , 64 , 96 , 96 , 128, 160, 255},
    {255, 192, 160, 128, 96 , 64 , 96 , 96 , 128, 160, 255, 255, 160, 128, 96 , 96 , 64 , 96 , 128, 160, 192, 255},
    {255, 255, 192, 160, 128, 94 , 96 , 128, 160, 192, 255, 255, 192, 160, 128, 96 , 94 , 128, 160, 192, 255, 255}
};
//these are the hues for the fire, 
//should be between 0 (red) to about 13 (yellow)
uint8_t EWCDisplay::_hueMask[M_HEIGHT][M_WIDTH] = {
    {1, 4, 7, 9, 13, 13, 10, 12, 8, 4, 1,     1, 4, 7, 9, 13, 13, 10, 12, 8, 4, 1},
    {1, 4, 7, 9, 9 , 13, 11, 10, 8, 4, 1,     1, 4, 7, 9, 9 , 13, 11, 10, 8, 4, 1},
    {1, 3, 5, 7, 9 , 9 , 10, 8 , 6, 4, 1,     1, 3, 5, 7, 9 , 9 , 10, 8 , 6, 4, 1},
    {1, 3, 5, 7, 9 , 9 , 8 , 6 , 4, 3, 1,     1, 3, 5, 7, 9 , 9 , 8 , 6 , 4, 3, 1},
    {1, 2, 5, 7, 8 , 8 , 7 , 6 , 3, 3, 1,     1, 2, 5, 7, 8 , 8 , 7 , 6 , 3, 3, 1},
    {0, 1, 3, 5, 6 , 7 , 7 , 5 , 3, 2, 1,     0, 1, 3, 5, 6 , 7 , 7 , 5 , 3, 2, 1},
    {0, 1, 4, 4, 5 , 6 , 6 , 4 , 3, 1, 0,     0, 1, 4, 4, 5 , 6 , 6 , 4 , 3, 1, 0},
    {0, 0, 1, 3, 4 , 4 , 4 , 2 , 2, 1, 0,     0, 0, 1, 3, 4 , 4 , 4 , 2 , 2, 1, 0},
    {0, 0, 0, 1, 2 , 3 , 2 , 2 , 1, 0, 0,     0, 0, 0, 1, 2 , 3 , 2 , 2 , 1, 0, 0},
    {0, 0, 0, 0, 1 , 1 , 1 , 0 , 0, 0, 0,     0, 0, 0, 0, 1 , 1 , 1 , 0 , 0, 0, 0}
};
uint8_t EWCDisplay::_matrix[M_WIDTH][M_HEIGHT];
uint8_t EWCDisplay::_line[M_WIDTH];
uint8_t EWCDisplay::_mapping[M_WIDTH][M_HEIGHT] = {
  {L0 , L1 , L2 , L3 , L4 , L5 , L6 , L7 , L8 , L9 },
  {L19, L18, L17, L16, L15, L14, L13, L12, L11, L10},
  {L20, L21, L22, L23, L24, L25, L26, L27, L28, L29},
  {L39, L38, L37, L36, L35, L34, L33, L32, L31, L30},
  {L40, L41, L42, L43, L44, L45, L46, L47, L48, L49},
  {L59, L58, L57, L56, L55, L54, L53, L52, L51, L50},
  {L60, L61, L62, L63, L64, L65, L66, L67, L68, L69},
  {L79, L78, L77, L76, L75, L74, L73, L72, L71, L70},
  {L80, L81, L82, L83, L84, L85, L86, L87, L88, L89},
  {L99, L98, L97, L96, L95, L94, L93, L92, L91, L90},
  {L100, L101, L102, L103, L104, L105, L106, L107, L108, L109},
  {L114, L115, L116, L117, L118, L119, L120, L121, L122, L123},
  {L133, L132, L131, L130, L129, L128, L127, L126, L125, L124},
  {L134, L135, L136, L137, L138, L139, L140, L141, L142, L143},
  {L153, L152, L151, L150, L149, L148, L147, L146, L145, L144},
  {L154, L155, L156, L157, L158, L159, L160, L161, L162, L163},
  {L173, L172, L171, L170, L169, L168, L167, L166, L165, L164},
  {L174, L175, L176, L177, L178, L179, L180, L181, L182, L183},
  {L193, L192, L191, L190, L189, L188, L187, L186, L185, L184},
  {L194, L195, L196, L197, L198, L199, L200, L201, L202, L203},
  {L213, L212, L211, L210, L209, L208, L207, L206, L205, L204},
  {L214, L215, L216, L217, L218, L219, L220, L221, L222, L223}
};
int EWCDisplay::_pcnt = 0;

void EWCDisplay::_setPixel(unsigned char x, unsigned char y, RgbColor rgb)
{
    int i = _mapping[x][y];
    _leds[i] = rgb;
}
/**
 * Randomly generate the next line (matrix row)
 */
void EWCDisplay::_generateLine()
{
  for(unsigned char x=0; x < M_WIDTH; x++) {
      _line[x] = random(64, 255);
  }
}
/**
 * shift all values in the matrix up one row
 */
void EWCDisplay::_shiftUp()
{
  for (unsigned char y=M_HEIGHT-1;y>0;y--) {
    for(unsigned char x=0;x<M_WIDTH;x++) {
        _matrix[x][y] = _matrix[x][y-1];
    }
  }
  
  for(unsigned char x=0;x<M_WIDTH;x++) {
      _matrix[x][0] = _line[x];
  }
}
/**
 * draw a frame, interpolating between 2 "key frames"
 * @param pcnt percentage of interpolation
 */
void EWCDisplay::_drawFrame(int pcnt)
{
  int nextv;
  unsigned char h;
  unsigned char s;
  unsigned char v;
  
  //each row interpolates with the one before it
  for (unsigned char y=M_HEIGHT-1;y>0;y--) {
    for (unsigned char x=0;x<M_WIDTH;x++) {
        h = _hueMask[y][x];
        s = 255;
        nextv = 
            (((100.0-pcnt)*_matrix[x][y] 
          + _pcnt*_matrix[x][y-1])/100.0) 
          - _valueMask[y][x];
        v = (unsigned char)max(0, nextv);
        _setPixel(x, y, _CHSV(h, s, v));
    }
  }
  
  //first row interpolates with the "next" line
  for(unsigned char x=0;x<M_WIDTH;x++) {
      h = _hueMask[0][x];
      s = 255;
      v = (unsigned char)(((100.0-_pcnt)*_matrix[x][0] + _pcnt*_line[x])/100.0);
      
      _setPixel(x, 0, _CHSV(h, s, v));
  }
}


EWCDisplay::EWCDisplay()
{

}

EWCDisplay::~EWCDisplay()
{
  
}

// TODO get rid of and replace with fastled function
RgbColor EWCDisplay::_CHSV(unsigned char inh, unsigned char ins, unsigned char inv)
{
  float r, g, b, h, s, v; //this function works with floats between 0 and 1
  float f, p, q, t;
  int i;

  RgbColor colorRGB;

  h = (float)(inh / 256.0);
  s = (float)(ins / 256.0);
  v = (float)(inv / 256.0);

  //if saturation is 0, the color is a shade of grey
  if(s == 0.0) {
    b = v;
    g = b;
    r = g;
  }
  //if saturation > 0, more complex calculations are needed
  else
  {
    h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
    i = (int)(floor(h)); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
    f = h - i;//the fractional part of h

    p = (float)(v * (1.0 - s));
    q = (float)(v * (1.0 - (s * f)));
    t = (float)(v * (1.0 - (s * (1.0 - f))));

    switch(i)
    {
      case 0: r=v; g=t; b=p; break;
      case 1: r=q; g=v; b=p; break;
      case 2: r=p; g=v; b=t; break;
      case 3: r=p; g=q; b=v; break;
      case 4: r=t; g=p; b=v; break;
      case 5: r=v; g=p; b=q; break;
      default: r = g = b = 0; break;
    }
  }
  
  return RgbColor((unsigned char)(r * 255.0), (unsigned char)(g * 255.0), (unsigned char)(b * 255.0));
}

void EWCDisplay::_resetStrip()
{
  _stackptr = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    _ledStrip[i] = 0;
  }
}

void EWCDisplay::_resetAndBlack()
{
  _resetStrip();
  for (int i = 0; i < NUM_LEDS; i++) {
    _leds[i] = _black;
  }
}

void EWCDisplay::_showStrip()
{
  for (int i = 0; i < NUM_LEDS; i++) {
    _ledBus.SetPixelColor(i, _leds[i]);
  }
  _ledBus.Show();
}

void EWCDisplay::_displayStrip(RgbColor colorCode)
{
  for (int i = 0; i < _stackptr; i++) {
    uint8_t j = _ledStrip[i];
    _leds[j] = colorCode;
  }
  _showStrip();
}

void EWCDisplay::_displayStrip()
{
  _displayStrip(_defaultColor);
}

void EWCDisplay::_displayStripRandomColor()
{
  for (int i = 0; i < _stackptr; i++) {
    _leds[_ledStrip[i]] = _CHSV(random(0, 255), 255, 255);
  }
  _showStrip();
}

void EWCDisplay::_pushToStrip(int ledId)
{
  _ledStrip[_stackptr] = ledId;
  _stackptr++;
}

void EWCDisplay::_pushToStrip(int leds[], uint8_t size)
{
  for(int i = 0; i < size; i++) {
    _pushToStrip(leds[i]);
  }
}

void EWCDisplay::_timeToStrip(uint8_t hours, uint8_t minutes)
{
  STRIP_PRINT(W1_ES_IST)

  if (minutes >= 5 && minutes < 10) {
    STRIP_PRINT(W1_FUENF1)
    STRIP_PRINT(W1_NACH)
  } 
  else if (minutes >= 10 && minutes < 15) {
    STRIP_PRINT(W1_ZEHN1)
    STRIP_PRINT(W1_NACH)
  } 
  else if (minutes >= 15 && minutes < 20) {
    STRIP_PRINT(W1_VIERTEL)
    STRIP_PRINT(W1_NACH)
  } 
  else if (minutes >= 20 && minutes < 25) {
    if (_languageMode == RHEIN_RUHR_MODE) {
      STRIP_PRINT(W1_ZWANZIG)
      STRIP_PRINT(W1_NACH)
    } 
    else if (_languageMode == WESSI_MODE) {
      STRIP_PRINT(W1_ZEHN1)
      STRIP_PRINT(W1_VOR)
      STRIP_PRINT(W1_HALB)
    }
  } 
  else if (minutes >= 25 && minutes < 30) {
    STRIP_PRINT(W1_FUENF1)
    STRIP_PRINT(W1_VOR)
    STRIP_PRINT(W1_HALB)
  } 
  else if (minutes >= 30 && minutes < 35) {
    STRIP_PRINT(W1_HALB)
  } 
  else if (minutes >= 35 && minutes < 40) {
    STRIP_PRINT(W1_FUENF1)
    STRIP_PRINT(W1_NACH)
    STRIP_PRINT(W1_HALB)
  } 
  else if (minutes >= 40 && minutes < 45) {
    if (_languageMode == RHEIN_RUHR_MODE) {
      STRIP_PRINT(W1_ZWANZIG)
      STRIP_PRINT(W1_VOR)
    } 
    else if (_languageMode == WESSI_MODE) {
      STRIP_PRINT(W1_ZEHN1)
      STRIP_PRINT(W1_NACH)
      STRIP_PRINT(W1_HALB)
    }
  } 
  else if (minutes >= 45 && minutes < 50) {
    STRIP_PRINT(W1_VIERTEL)
    STRIP_PRINT(W1_VOR)
  } 
  else if (minutes >= 50 && minutes < 55) {
    STRIP_PRINT(W1_ZEHN1)
    STRIP_PRINT(W1_VOR)
  } 
  else if (minutes >= 55 && minutes < 60) {
    STRIP_PRINT(W1_FUENF1)
    STRIP_PRINT(W1_VOR)
  }

  int singleMinutes = minutes % 5;
  switch (singleMinutes) {
    case 1:
      STRIP_PRINT(W1_ONE)
      break;
    case 2:
      STRIP_PRINT(W1_ONE)
      STRIP_PRINT(W1_TWO)
      break;
    case 3:
      STRIP_PRINT(W1_ONE)
      STRIP_PRINT(W1_TWO)
      STRIP_PRINT(W1_THREE)
      break;
    case 4:
      STRIP_PRINT(W1_ONE)
      STRIP_PRINT(W1_TWO)
      STRIP_PRINT(W1_THREE)
      STRIP_PRINT(W1_FOUR)
      break;
  }

  if (hours >= 12) {
    hours -= 12;
  }

  if (_languageMode == RHEIN_RUHR_MODE) {
    if (minutes >= 25) {
      hours++;
    }
  } 
  else if (_languageMode == WESSI_MODE) {
    if (minutes >= 20) {
      hours++;
    }
  }

  if (hours == 12) {
    hours = 0;
  }

  //show hours
  switch (hours) {
    case 0:
      STRIP_PRINT(W1_ZWOELF)
      break;
    case 1:
      if (minutes > 4) {
        STRIP_PRINT(W1_EINS)
      } else {
        STRIP_PRINT(W1_EIN)
      }
      break;
    case 2:
      STRIP_PRINT(W1_ZWEI)
      break;
    case 3:
      STRIP_PRINT(W1_DREI)
      break;
    case 4:
      STRIP_PRINT(W1_VIER)
      break;
    case 5:
      STRIP_PRINT(W1_FUENF2)
      break;
    case 6:
      STRIP_PRINT(W1_SECHS)
      break;
    case 7:
      STRIP_PRINT(W1_SIEBEN)
      break;
    case 8:
      STRIP_PRINT(W1_ACHT)
      break;
    case 9:
      STRIP_PRINT(W1_NEUN)
      break;
    case 10:
      STRIP_PRINT(W1_ZEHN)
      break;
    case 11:
      STRIP_PRINT(W1_ELF)
      break;
  }

  //show uhr
  if (minutes < 5) {
    STRIP_PRINT(W1_UHR)
  }
}

#if FEATURE_WEATHER()
void EWCDisplay::_weatherToStrip(int8_t temperature, int8_t weather)
{
  STRIP_PRINT(W2_HEUTE)
  STRIP_PRINT(W2_WIRD)
  STRIP_PRINT(W2_ES)

  if (temperature < 0) {
    STRIP_PRINT(W2_MINUS)
  }

  switch ((int)(abs(temperature) / 5)) {
    case 0:
      STRIP_PRINT(W2_NULL)
      break;
    case 1:
      STRIP_PRINT(W2_FUENF)
      break;   
    case 2:
      STRIP_PRINT(W2_ZEHN)
      break;
    case 3:
      STRIP_PRINT(W2_FUENF)
      STRIP_PRINT(W2_ZEHN)
      break;
    case 4:
      STRIP_PRINT(W2_ZWANZIG)
      break;
    case 5:
      STRIP_PRINT(W2_FUENF)
      STRIP_PRINT(W2_UND)
      STRIP_PRINT(W2_ZWANZIG)
      break;
    case 6:
      STRIP_PRINT(W2_DREISSIG)
      break;
    case 7:
      STRIP_PRINT(W2_FUENF)
      STRIP_PRINT(W2_UND)
      STRIP_PRINT(W2_DREISSIG)
      break;
  }

  switch (abs(temperature) % 5) {
    case 1:
      STRIP_PRINT(W2_ONE)
      break;
    case 2:
      STRIP_PRINT(W2_ONE)
      STRIP_PRINT(W2_TWO)
      break;
    case 3:
      STRIP_PRINT(W2_ONE)
      STRIP_PRINT(W2_TWO)
      STRIP_PRINT(W2_THREE)
      break;
    case 4:
      STRIP_PRINT(W2_ONE)
      STRIP_PRINT(W2_TWO)
      STRIP_PRINT(W2_THREE)
      STRIP_PRINT(W2_FOUR)
      break;
  }

  if (abs(temperature) > 39) {
      STRIP_PRINT(W2_FUENF)
      STRIP_PRINT(W2_UND)
      STRIP_PRINT(W2_DREISSIG)
      STRIP_PRINT(W2_ONE)
      STRIP_PRINT(W2_TWO)
      STRIP_PRINT(W2_THREE)
      STRIP_PRINT(W2_FOUR)
  }

  STRIP_PRINT(W2_GRAD)
  
  if (weather > 0) {
    STRIP_PRINT(W2_UND2)

    switch (weather) {
      case WEATHER_SONNE:
        STRIP_PRINT(W2_SONNE)
        break;
      case WEATHER_NEBEL:
        STRIP_PRINT(W2_NEBEL)
        break;
      case WEATHER_WOLKEN:
        STRIP_PRINT(W2_WOLKEN)
        break;
      case WEATHER_SONNEMITWOLKEN:
        STRIP_PRINT(W2_SONNE)
        STRIP_PRINT(W2_MIT)
        STRIP_PRINT(W2_WOLKEN)
        break;      
      case WEATHER_NIESEL:
        STRIP_PRINT(W2_NIESEL)
        break;
      case WEATHER_SCHNEE:
        STRIP_PRINT(W2_SCHNEE)
        break;       
      case WEATHER_REGEN:
        STRIP_PRINT(W2_REGEN)
        break;
      case WEATHER_SCHNEEREGEN:
        STRIP_PRINT(W2_SCHNEE)
        STRIP_PRINT(W2_REGEN)
        break;
      case WEATHER_STURM:
        STRIP_PRINT(W2_STURM)
        break; 
    }
    STRIP_PRINT(W2_GEBEN)   
  }

}
#endif

bool EWCDisplay::begin()
{
  
  for (int i = 0; i < NUM_LEDS; i++) {
    _ledStrip[i] = 0;
  }
  _ledBus.Begin();
  _resetAndBlack();
  _displayStrip();
  
  return true;
}

void EWCDisplay::setAutoBrightness(boolean enabled)
{
  _autoBrightness = enabled;
}

boolean EWCDisplay::getAutoBrightness() {
  return _autoBrightness;
}

void EWCDisplay::changeBrightness(int8_t amount)
{
  RgbColor prevColor;
  uint8_t speed = 100;
  
  _autoBrightness = false;
  if (amount > 0) {
    _defaultColor.Lighten(_defaultColor.CalculateBrightness() / amount);
  } 
  else {
    _defaultColor.Darken(_defaultColor.CalculateBrightness() / abs(amount));
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    prevColor = _ledBus.GetPixelColor(i);
    if (amount > 0) {
      prevColor.Lighten(prevColor.CalculateBrightness() / amount);
    }
    else {
      prevColor.Darken(prevColor.CalculateBrightness() / abs(amount));
    }
    _ledBus.LinearFadePixelColor(speed, i, prevColor);
  }
  _ledBus.StartAnimating();
}

void EWCDisplay::setBrightness(uint8_t value)
{
  RgbColor prevColor;
  uint8_t speed = 100;
  int8_t amount;

  if(value != _oldBrightness) {
    amount = value - _oldBrightness;
    _oldBrightness = value;
    if (amount > 0) {
      _defaultColor.Lighten(_defaultColor.CalculateBrightness() / amount);
    } 
    else {
      _defaultColor.Darken(_defaultColor.CalculateBrightness() / abs(amount));
    }
    for (int i = 0; i < NUM_LEDS; i++) {
      prevColor = _ledBus.GetPixelColor(i);
      if (amount > 0) {
        prevColor.Lighten(prevColor.CalculateBrightness() / amount);
      } 
      else {
        prevColor.Darken(prevColor.CalculateBrightness() / abs(amount));
      }
      _ledBus.LinearFadePixelColor(speed, i, prevColor);
    }
    _ledBus.StartAnimating();
  }
}

void EWCDisplay::off()
{
  DEBUG_PRINTLN(F("switching off"));
  if (!_ledBus.IsAnimating()) {
    _resetAndBlack();
    for (int i = 0; i < NUM_LEDS; i++) {
      _ledBus.LinearFadePixelColor(1000, i, _black);
    }
    _ledBus.StartAnimating();
  }
}

void EWCDisplay::showHeart()
{
  DEBUG_PRINT(F("HEART..."));
  if (!_ledBus.IsAnimating()) {
    _autoBrightness = false;
    _resetAndBlack();
    STRIP_PRINT(W1_HEART)
    #if FEATURE_WEATHER()
    STRIP_PRINT(W2_HEART)
    #endif
    _displayStrip();
  }
}

void EWCDisplay::testAll()
{
  DEBUG_PRINTLN(F("SNAKE..."));
  if (!_ledBus.IsAnimating()) {
    _autoBrightness = false;
    _resetAndBlack();
    _leds[_testLED] = _defaultColor;
    RgbColor c;
    c = _defaultColor;
    c.Darken(30);
    if (_testLED > 0) _leds[_testLED - 1] = c;
    if (_testLED < NUM_LEDS-1) _leds[_testLED + 1] = c;
    c.Darken(30);
    if (_testLED > 1) _leds[(_testLED - 2)] = c;
    c.Darken(30);
    if (_testLED > 2) _leds[_testLED - 3] = c;
    _showStrip();

    _leds[_testLED] = _black;
    if (_testLED > 0) _leds[_testLED - 1] = _black;
    if (_testLED > 1) _leds[(_testLED - 2)] = _black;
    if (_testLED > 2) _leds[_testLED - 3] = _black;
    if (_testLED < NUM_LEDS-1) _leds[_testLED + 1] = _black;

    _testLED++;
    if (_testLED >= NUM_LEDS) {
      _testLED = 0;
    }
  }
}

void EWCDisplay::fastTest()
{
  DEBUG_PRINTLN(F("FAST TEST..."));
  if (!_ledBus.IsAnimating()) {
    _autoBrightness = false;
    _resetAndBlack();
    if (_testMinutes >= 60) {
      _testMinutes = 0;
      _testHours++;
    }
    if (_testHours >= 24) {
      _testHours = 0;
    }
    _timeToStrip(_testHours, _testMinutes);
    _testMinutes++;
    #if FEATURE_WEATHER()
    if (_testTemperature > 30) {
      _testTemperature = -39;
      _testWeather++;
      if (_testWeather > 9) {
        _testWeather = 0;
      }    
    }
    _weatherToStrip(_testTemperature, _testWeather);
    _testTemperature++;
    #endif
    _displayStripRandomColor();
  }
}

void EWCDisplay::makeParty()
{
  DEBUG_PRINTLN(F("PARTY YEAH..."));
  if (!_ledBus.IsAnimating()) {
    _autoBrightness = false;
    _resetAndBlack();
    for (int i = 0; i < NUM_LEDS; i++) {
      //leds[i] = CHSV(random(0, 255), 255, 255);
      _leds[i] = RgbColor(random(0, 100), random(0, 100), random(0, 100));
    }
    _showStrip();
  }
}

void EWCDisplay::fire()
{
  DEBUG_PRINTLN(F("Fire..."));
  if (!_ledBus.IsAnimating()) {
    _autoBrightness = false;
    _resetAndBlack();
    
    if (_pcnt >= 100){
        _shiftUp();
        _generateLine();
        _pcnt = 0;
    }
    _drawFrame(_pcnt);
    _pcnt+=30;
    
    _showStrip();
  }
}

void EWCDisplay::clockLogic()
{
  DEBUG_PRINTLN(F("Clock..."));
  if (!_ledBus.IsAnimating()) {
    _resetAndBlack();
    time_t now = time(nullptr);
    _timeToStrip(localtime(&now)->tm_hour, localtime(&now)->tm_min);
    #if FEATURE_WEATHER()
    _weatherToStrip(Weather.getTemperature(), Weather.getWeather());
    #endif
    _displayStrip(_defaultColor);
  }
}

void EWCDisplay::setDisplay(FunctPtr fp)
{
  _displayInterval = 1000;
  _currentDisplay = fp;
  _currentDisplayId = 0;
  if (_currentDisplay == clockLogic) {
    _displayInterval = 10000;
    _defaultColor = _white;
    _currentDisplayId = 6;
  }
  else if (_currentDisplay == off) {
    _displayInterval = 60000; // we want to run it just once, so a high value
    _currentDisplayId = 0;
  }
  else if (_currentDisplay == fastTest) {
    _displayInterval = 1000;
    _currentDisplayId = 3;
  }
  else if (_currentDisplay == makeParty) {
    _displayInterval = 120;
    _currentDisplayId = 4;
  }   
  else if (_currentDisplay == showHeart) {
    _displayInterval = 10000;
    _defaultColor = _red;
    _currentDisplayId = 1;
  }
  else if (_currentDisplay == testAll) {
    _displayInterval = 120;
    _defaultColor = _red;
    _currentDisplayId = 2;
  }
  else if (_currentDisplay == fire) {
    _displayInterval = 120;
    _currentDisplayId = 5;
    randomSeed(analogRead(0));
    _generateLine();
  
    //init all pixels to zero
    for (unsigned char y=0;y<M_HEIGHT;y++) {
      for(unsigned char x=0;x<M_WIDTH;x++) {
        _matrix[x][y] = 0;
      }
    }
  }
  (_currentDisplay)();
}

uint8_t EWCDisplay::getDisplay()
{
  return _currentDisplayId;
}

const char **EWCDisplay::getDisplayList()
{
  return _displayList;
}

void EWCDisplay::setDisplay(uint8_t displayId)
{
  switch(displayId) {
    case 0: 
      setDisplay(off);
      break;
    case 1: 
      setDisplay(showHeart);
      break;
    case 2: 
      setDisplay(testAll);
      break;
    case 3: 
      setDisplay(fastTest);
      break;
    case 4: 
      setDisplay(makeParty);
      break;
    case 5: 
      setDisplay(fire);
      break;
    case 6: 
      setDisplay(clockLogic);
      break;
  }  
}

void EWCDisplay::setColor(unsigned char r, unsigned char g, unsigned char b)
{
  _defaultColor = RgbColor(r, g, b);
  (_currentDisplay)();
}

RgbColor EWCDisplay::getColor()
{
  return _defaultColor;
}

void EWCDisplay::handle()
{
  // Are we in animation mode?
  if (_ledBus.IsAnimating()) {
    _ledBus.UpdateAnimations();
    _ledBus.Show();
  }
  else {
    // nextrun overdue
    if (_lastRun + _displayInterval < millis()) {
      DEBUG_MSG("");
      (_currentDisplay)();
      _lastRun = millis();
    }
  }
}

EWCDisplay Display = EWCDisplay();
