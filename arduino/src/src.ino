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
#define STRIP_UP 2
#define STRIP_RED 3
#define STRIP_YELLOW 4
#define STRIP_GREEN 5
#define STRIP_BLUE 6


#define ON_OFF_DURATION 1000
#define ON_OFF_FADE 1

#define MODE_DURATION 5000
#define SHORT_FADE_DURATION 250
#define MEDIUM_FADE_DURATION 750
#define LONG_FADE_DURATION 1500

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

  heartEase.setDuration(SHORT_FADE_DURATION);
  heartEase.setTotalChangeInPosition(255);
  
  strip.begin();
  
  selectStripMode('n');
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
  stripModeStart = millis();
    switch (mode) {
      case 'f':
          stripMode = STRIP_OFF;
        break;
      case 'n':
          stripMode = STRIP_ON;
        break;
      case 'u':
          stripMode = STRIP_UP;
        break;
      case '1':
          stripMode = STRIP_RED;
          strip.clear();
        break;
      case '2':
          stripMode = STRIP_YELLOW;
          strip.clear();
        break;
      case '3':
          stripMode = STRIP_GREEN;
          strip.clear();
        break;
      case '4':
          stripMode = STRIP_BLUE;
          strip.clear();
        break;
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
        if ( timePassed < ON_OFF_DURATION) {
          for(uint8_t i=0; i<strip.numPixels(); i++) {
            int val = 255 - onOffEase.easeOut(timePassed) - i * ON_OFF_FADE;
            strip.setPixelColor(i, strip.Color(0,0,0,constrain(val, 0, 255)));
          }
          strip.show();
        }
        break;
      case STRIP_ON:
        if ( timePassed < ON_OFF_DURATION) {
          for(uint8_t i=0; i<strip.numPixels(); i++) {
            int val = onOffEase.easeIn(timePassed) - i * ON_OFF_FADE;
            strip.setPixelColor(i, strip.Color(0,0,0,constrain(val, 0, 255)));
          }
          strip.show();
        }
        break;
      case STRIP_UP:
      {
        uint8_t len = 30;
        uint8_t str = 10;
        
        uint8_t minLight = (currentTime / 10) % len;
        uint8_t maxLight = (currentTime / 10 + str) % len;
        
        for(uint8_t i=0; i<strip.numPixels(); i++) {
          uint8_t light = i % len;
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
        break;
      case STRIP_RED:
        break;
      case STRIP_YELLOW:
        break;
      case STRIP_GREEN:
        break;
      case STRIP_BLUE:
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


