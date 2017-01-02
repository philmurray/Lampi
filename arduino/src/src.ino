#include <EasingLibrary.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIXEL_PIN 6

#define BOT_1_PIN 17
#define BOT_2_PIN 16
#define BOT_3_PIN 15
#define BOT_4_PIN 14

#define TOP_1_PIN 9
#define TOP_2_PIN 5
#define TOP_3_PIN 3


#define BOT_1 0
#define BOT_2 1
#define BOT_3 2
#define BOT_4 3

#define TOP_1 4
#define TOP_2 5
#define TOP_3 6

#define STRIP 7

#define BUTTON_OFF 0
#define BUTTON_ON 1
#define BUTTON_BLINK 2
#define BUTTON_FASTBLINK 3

#define STRIP_OFF 0
#define STRIP_ON 1
#define STRIP_FAST_ON 2
#define STRIP_UP 3
#define STRIP_RED 4
#define STRIP_YELLOW 5
#define STRIP_GREEN 6
#define STRIP_BLUE 7

#define UP_SPEED 25
#define UP_LENGTH 5
#define UP_REPEAT 50

#define ON_OFF_DURATION 1000
#define ON_OFF_FADE 1

#define MODE_DURATION 5000
#define SHORT_FADE_DURATION 250
#define MEDIUM_FADE_DURATION 750
#define LONG_FADE_DURATION 1500

#define HEARTBEAT_DURATION 300

#define SUN_DURATION 2000
#define SUN_FADE 30

int buttons[][2] = {
  {BOT_1_PIN, 2}, 
  {BOT_2_PIN, 0}, 
  {BOT_3_PIN, 0}, 
  {BOT_4_PIN, 0}, 
  {TOP_1_PIN, 0}, 
  {TOP_2_PIN, 0}, 
  {TOP_3_PIN, 0}
};
int numButtons = 7;

int stripMode = 0;
long stripModeStart = 0;
int stripWidth = 10;
int stripHeight = 6;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);
char serial_char = 0;
bool starting_up = true;

int selected = -1;

QuarticEase onOffEase;
SineEase shortEase;
SineEase mediumEase;
SineEase longEase;

CircularEase heartEase;
SineEase sunEase;

void setup(){
  Serial.begin(9600);
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttons[i][0], OUTPUT);
  }

  onOffEase.setDuration(ON_OFF_DURATION);
  onOffEase.setTotalChangeInPosition(255 + ON_OFF_FADE * 60);

  shortEase.setDuration(SHORT_FADE_DURATION);
  shortEase.setTotalChangeInPosition(255);
  
  mediumEase.setDuration(MEDIUM_FADE_DURATION);
  mediumEase.setTotalChangeInPosition(255);
  
  longEase.setDuration(LONG_FADE_DURATION);
  longEase.setTotalChangeInPosition(255);

  heartEase.setDuration(HEARTBEAT_DURATION);
  heartEase.setTotalChangeInPosition(255);
  
  sunEase.setDuration(SUN_DURATION);
  sunEase.setTotalChangeInPosition(255 + SUN_FADE * 6);
  
  strip.begin();
}

void loop(){
  if (starting_up) {
    startup();
  }
  if (Serial.available()) {
    serial_char = Serial.read();
    if (selected == -1) {
      selectThing(serial_char);
      Serial.write(serial_char);
    } else {
      selectMode(serial_char);
      Serial.write(serial_char);
    }
  }
  displayButtons();
  displayStrip();
}

void startup() {
  for (int i = 0; i < numButtons; i++) {
    digitalWrite(buttons[i][0], HIGH);
    delay(100);
    digitalWrite(buttons[i][0], LOW);
  }
  starting_up = false;
  
  selectStripMode('m');
}

void selectThing(char thing) {
  switch (thing) {
    case '1':
      selected = BOT_1;
      break;
    case '2':
      selected = BOT_2;
      break;
    case '3':
      selected = BOT_3;
      break;
    case '4':
      selected = BOT_4;
      break;
    case 'a':
      selected = TOP_1;
      break;
    case 'b':
      selected = TOP_2;
      break;
    case 'c':
      selected = TOP_3;
      break;
    case 's':
      selected = STRIP;
      break;
  }
}

void selectMode(char mode) {
  if (selected < numButtons) {
    selectButtonMode(mode);
  } else if (selected == STRIP) {
    selectStripMode(mode);
  }

  selected = -1;
}

void selectStripMode(char mode) {
  int newStripMode = 0;
  
  switch (mode) {
    case 'f':
      newStripMode = STRIP_OFF;
      break;
    case 'm':
      newStripMode = STRIP_ON;
      break;
    case 'n':
      newStripMode = STRIP_FAST_ON;
      for(uint8_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0,0,0,255));
      }
      strip.show();
      break;
    case 'u':
      newStripMode = STRIP_UP;
      break;
    case '1':
      newStripMode = STRIP_RED;
      break;
    case '2':
      newStripMode = STRIP_YELLOW;
      break;
    case '3':
      newStripMode = STRIP_GREEN;
      break;
    case '4':
      newStripMode = STRIP_BLUE;
      break;
  }
  if (stripMode != newStripMode) {
    stripMode = newStripMode;
    stripModeStart = millis();
    strip.clear();
    strip.show();
  }
}

void selectButtonMode(char mode) {
    switch (mode) {
      case 'n':
          buttons[selected][1] = BUTTON_ON;
          digitalWrite(buttons[selected][0], HIGH);
        break;
      case 'f':
          buttons[selected][1] = BUTTON_OFF;
          digitalWrite(buttons[selected][0], LOW);
        break;
      case 'b':
          buttons[selected][1] = BUTTON_BLINK;
        break;
      case 's':
          buttons[selected][1] = BUTTON_FASTBLINK;
        break;
    }
}

void displayStrip() {
    unsigned long currentTime = millis();
    unsigned long timePassed = currentTime - stripModeStart;
    switch (stripMode) {
      case STRIP_OFF:
        displayOff(currentTime, timePassed);
        break;
      case STRIP_ON:
        displayOn(currentTime, timePassed);
        break;
      case STRIP_UP:
        displayUp(currentTime, timePassed);
        break;
      case STRIP_RED:
        displayRed(currentTime, timePassed);
        break;
      case STRIP_YELLOW:
        displayYellow(currentTime, timePassed);
        break;
      case STRIP_GREEN:
        displayGreen(currentTime, timePassed);
        break;
      case STRIP_BLUE:
        displayBlue(currentTime, timePassed);
        break;
    }
}

void displayButtons() {
  for (int i = 0; i < numButtons; i++) {
    switch (buttons[i][1]) {
      case BUTTON_BLINK:
        if (millis() % 500 < 250) {
          digitalWrite(buttons[i][0], HIGH);
        }
        else {
          digitalWrite(buttons[i][0], LOW);
        }
      break;
      case BUTTON_FASTBLINK:
        if (millis() % 200 < 100) {
          digitalWrite(buttons[i][0], HIGH);
        }
        else {
          digitalWrite(buttons[i][0], LOW);
        }
      break;
    }
  }
}

void displayUp(unsigned long currentTime, unsigned long timePassed)
{
  uint8_t minLight = (currentTime / UP_SPEED) % UP_REPEAT;
  uint8_t maxLight = (currentTime / UP_SPEED + UP_LENGTH) % UP_REPEAT;
  
  for(uint8_t i=0; i<strip.numPixels(); i++) {
    uint8_t light = i % UP_REPEAT;
    if ((minLight < maxLight && light > minLight && light <= maxLight) || (minLight > maxLight && (light <= maxLight || light > minLight)))
    {
      strip.setPixelColor(i, strip.Color(0,0,0,255));
    }
    else
    {
      strip.setPixelColor(i, strip.Color(0,0,0,0));
    }
  }
  strip.show();
}

void displayOn(unsigned long currentTime, unsigned long timePassed)
{
  if ( timePassed < ON_OFF_DURATION) {
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      int val = onOffEase.easeIn(timePassed) - i * ON_OFF_FADE;
      strip.setPixelColor(i, strip.Color(0,0,0,constrain(val, 0, 255)));
    }
    strip.show();
  }
}
void displayOff(unsigned long currentTime, unsigned long timePassed)
{
  if ( timePassed < ON_OFF_DURATION) {
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      int val = 255 - constrain(onOffEase.easeOut(timePassed) - i * ON_OFF_FADE, 0, 255);
      strip.setPixelColor(i, strip.Color(0,0,0,val));
    }
    strip.show();
  }
}
void displayRed(unsigned long currentTime, unsigned long timePassed)
{
  timePassed = timePassed % (HEARTBEAT_DURATION * 5);
  
  if (timePassed > 0 && timePassed < HEARTBEAT_DURATION) 
  {
    
  }
  else
  {
    timePassed -= HEARTBEAT_DURATION;
  }
  
  if (timePassed > 0 && timePassed < HEARTBEAT_DURATION)
  {
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      int val = heartEase.easeIn(timePassed);
      strip.setPixelColor(i, strip.Color(0,constrain(val, 0, 255),0,0));
    }
  }
  else
  {
    timePassed -= HEARTBEAT_DURATION;
  }
  
  if (timePassed > 0 && timePassed > HEARTBEAT_DURATION)
  {
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      int val = 255 - heartEase.easeOut(timePassed);
      strip.setPixelColor(i, strip.Color(0, constrain(val, 0, 255),0,0));
    }
  }
  else
  {
    timePassed -= HEARTBEAT_DURATION;
  }
  
  if (timePassed > 0 && timePassed < HEARTBEAT_DURATION)
  {
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      int val = heartEase.easeIn(timePassed);
      strip.setPixelColor(i, strip.Color(0, constrain(val, 0, 255),0,0));
    }
  }
  else
  {
    timePassed -= HEARTBEAT_DURATION;
  }
  
  if (timePassed > 0 && timePassed > HEARTBEAT_DURATION)
  {
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      int val = 255 - heartEase.easeOut(timePassed);
      strip.setPixelColor(i, strip.Color(0, constrain(val, 0, 255),0,0));
    }
  }
  else
  {
    timePassed -= HEARTBEAT_DURATION;
  }
  
  strip.show();
}

void displayYellow(unsigned long currentTime, unsigned long timePassed)
{
  if ( timePassed < SUN_DURATION * 1.5) {
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      int val = sunEase.easeIn(timePassed) - (int(i / 10) * SUN_FADE);
      strip.setPixelColor(i, strip.Color(constrain(val, 0, 255) * 0.5f,constrain(val, 0, 255),0,0));
    }
  }

  if ( timePassed > SUN_DURATION * 1.5 && timePassed < MODE_DURATION) {
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      int val = 255 - constrain(sunEase.easeOut(timePassed - SUN_DURATION * 1.5) - (int(((strip.numPixels() - 1) - i) / 10) * SUN_FADE), 0, 255);
      strip.setPixelColor(i, strip.Color(val * 0.5f,val,0,0));
    }
  }
  strip.show();
}

void displayGreen(unsigned long currentTime, unsigned long timePassed)
{
  uint8_t step = MODE_DURATION / strip.numPixels();
  
  if ( timePassed < MODE_DURATION / 2)
  {
    for(uint8_t i=0; i<strip.numPixels()/2; i++) {
      if (timePassed > i * step) {
        strip.setPixelColor(i, strip.Color(255,0,0,0));
        strip.setPixelColor((strip.numPixels() - 1) - i, strip.Color(255,0,0,0));
      }
    }
  }
  else
  {    
    for(uint8_t i=0; i<strip.numPixels()/2; i++) {
      int s = strip.numPixels()/2 - 1 - i;
      if ((timePassed - MODE_DURATION / 2) > s * step) {
        strip.setPixelColor(i, strip.Color(0,0,0,0));
        strip.setPixelColor((strip.numPixels() - 1) - i, strip.Color(0,0,0,0));
      }
    }
  }
  strip.show();
}

void displayBlue(unsigned long currentTime, unsigned long timePassed)
{
  if (timePassed < MODE_DURATION - MEDIUM_FADE_DURATION)
  {
    //pick a random pixel every ... some amount of time.  turn it on
    if (timePassed % 10 == 0)
    {
      int i = random(0, strip.numPixels());
      strip.setPixelColor(i, strip.Color(0, 0, 255 ,0));
    }

    //cool stuff
  }
  else if (timePassed > MODE_DURATION - MEDIUM_FADE_DURATION && timePassed < MODE_DURATION)
  {
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      int val = 255 - constrain(mediumEase.easeOut(timePassed - (MODE_DURATION - MEDIUM_FADE_DURATION)), 0, 255);
      strip.setPixelColor(i, strip.Color(0, 0, val ,0));
    }
  }
  strip.show();
}

