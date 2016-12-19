#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIXEL_PIN 6

#define BOT_1_PIN 14
#define BOT_2_PIN 15
#define BOT_3_PIN 16
#define BOT_4_PIN 17

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
#define STRIP_WHITE_255 1
#define STRIP_WHITE_75 2
#define STRIP_WHITE_25 3

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

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);
char serial_char = 0;
bool starting_up = true;

int selected = -1;


void setup(){
  Serial.begin(9600);
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttons[i][0], OUTPUT);
  }
  
  strip.begin();
  
  selectStripMode('2'); //TODO: load from memory
}

void loop(){
  if (starting_up) {
    startup();
  }
  if (Serial.available()) {
    serial_char = Serial.read();
    if (selected == -1) {
      selectThing(serial_char);
    } else {
      selectMode(serial_char);
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
    switch (mode) {
      case 'f':
          stripMode = STRIP_OFF;
        break;
      case '1':
          stripMode = STRIP_WHITE_255;
          all_white(255);
        break;
      case '2':
          stripMode = STRIP_WHITE_75;
          all_white(75);
        break;
      case '3':
          stripMode = STRIP_WHITE_25;
          all_white(25);
        break;
    }
}

void selectButtonMode(char mode) {
    switch (mode) {
      case 'n':
          buttons[selected][1] = BUTTON_ON;
        break;
      case 'f':
          buttons[selected][1] = BUTTON_OFF;
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
    switch (stripMode) {
      case STRIP_OFF:
        break;
      case STRIP_WHITE_255:
        break;
      case STRIP_WHITE_75:
        break;
      case STRIP_WHITE_25:
        break;
    }
}

void displayButtons() {
  for (int i = 0; i < numButtons; i++) {
    switch (buttons[i][1]) {
      case BUTTON_OFF:
        digitalWrite(buttons[i][0], LOW);
      break;
      case BUTTON_ON:
        digitalWrite(buttons[i][0], HIGH);
      break;
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
  starting_up = false;
}

void all_white(uint8_t b) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,0,0,b));
    strip.show();
  }
}


