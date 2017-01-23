#include "PinState.h"
#include "OnOffPinState.h"
#include "BlinkPinState.h"
#include "StripState.h"
#include "StripStateStep.h"

#include "Ease.h"
#include "LinearEase.h"
#include "SineEase.h"

#include "Selector.h"
#include "BlockSelector.h"
#include "RowWipeSelector.h"
#include "ColWipeSelector.h"
#include "RowRunnerSelector.h"
#include "ColRunnerSelector.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIXEL_PIN 6
#define STRIP_PINS 60

#define BOT_1_PIN 17
#define BOT_2_PIN 16
#define BOT_3_PIN 15
#define BOT_4_PIN 14

#define TOP_1_PIN 9
#define TOP_2_PIN 5
#define TOP_3_PIN 3

#define NUM_PINS 7
#define STRIP_STATE 7
#define NO_SELECTION 254

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_PINS, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);
char serial_char = 0;
bool starting_up = true;
byte selectedPin = NO_SELECTION;

byte pins[] = {BOT_1_PIN, BOT_2_PIN, BOT_3_PIN, BOT_4_PIN, TOP_1_PIN, TOP_2_PIN, TOP_3_PIN};
PinState* pinStates[7];

OnOffPinState OnPin = OnOffPinState(true);
OnOffPinState OffPin = OnOffPinState(false);
BlinkPinState SlowBlink = BlinkPinState(500, 250);
BlinkPinState FastBlink = BlinkPinState(200, 100);

StripState* stripState;


Selector AllSelector = Selector();

Ease OnNow = Ease(0, 255, 0, false);
StripStateStep On[]  = {
  {0,-1,&AllSelector, 0, 0, 0, &OnNow}
};
StripState OnState = StripState(&strip, On, sizeof(On) / sizeof(On[0]));

Ease OffNow = Ease(0, 0, 0, false);
StripStateStep Off[]  = {
  {0,-1,&AllSelector, 0, 0, 0, &OffNow }
};
StripState OffState = StripState(&strip, Off, sizeof(Off) / sizeof(On[0]));


RowRunnerSelector UpR = RowRunnerSelector(0, 5, 0, 9, 60, 3, 6, false);
StripStateStep Up[] = {
	{0,-1,&UpR, 0,0,0,&OnNow}
};
StripState UpState = StripState(&strip, Up, sizeof(Up) / sizeof(On[0]));

Ease s1e1 = Ease(0, 10, 0, false);
SineEase s1e2 = SineEase(0, 255, 250, true);
StripStateStep State_1_steps[] = {
	{ 0, -1, &AllSelector, 0, 0, &s1e1, &s1e1 },
	{ 500, 1000, &AllSelector, 0, 0, &s1e2, 0 },
	{ 2250, 1000, &AllSelector, 0, 0, &s1e2, 0 },
	{ 4000, 1000, &AllSelector, 0, 0, &s1e2, 0 }
};
StripState State_1 = StripState(&strip, State_1_steps, sizeof(State_1_steps) / sizeof(On[0]));

LinearEase s2e3 = LinearEase(0, 255, 2000, false);
LinearEase s2e4 = LinearEase(0, 120, 2000, false);
RowWipeSelector s2s1 = RowWipeSelector(0, 5, 0, 9, 2000, true, false);
RowWipeSelector s2s2 = RowWipeSelector(0, 5, 0, 9, 2000, true, true);
StripStateStep State_2_steps[] = {
	{ 0,4000,&s2s1, &s2e4, 0,&s2e3,0 },
	{ 4000,4000,&s2s2, &s2e4, 0, &s2e3, 0 }
};
StripState State_2 = StripState(&strip, State_2_steps, sizeof(State_2_steps) / sizeof(On[0]));

SineEase s3e1 = SineEase(0, 255, 750, false);
SineEase s3e2 = SineEase(0, 120, 750, false);
SineEase s3e3 = SineEase(0, 0, 750, false);
SineEase s3e4 = SineEase(0, 50, 750, false);
RowWipeSelector s3s1 = RowWipeSelector(0, 5, 4, 4, 2000, true, false);
ColWipeSelector s3s2 = ColWipeSelector(0, 5, 0, 4, 2000, false, false);
ColWipeSelector s3s3 = ColWipeSelector(0, 5, 4, 9, 2000, true, false);
StripStateStep State_3_steps[] = {
	{ 0,-1,&s3s1, &s3e2, &s3e3, &s3e4,0 },
	{ 2000,-1,&s3s2, &s3e1, 0,0,0 },
	{ 2000,-1,&s3s3, &s3e1, 0,0,0 }
};
StripState State_3 = StripState(&strip, State_3_steps, sizeof(State_3_steps) / sizeof(On[0]));

LinearEase s4e1 = LinearEase(0, 200, 4000, false);
LinearEase s4e2 = LinearEase(0, 10, 4000, false);
RowRunnerSelector s4s1 = RowRunnerSelector(0, 5, 5, 5, 100, 1, 7, true);
RowRunnerSelector s4s2 = RowRunnerSelector(0, 5, 3, 3, 100, 1, 13, true);
RowRunnerSelector s4s3 = RowRunnerSelector(0, 5, 4, 4, 100, 1, 19, true);
RowRunnerSelector s4s4 = RowRunnerSelector(0, 5, 1, 1, 100, 1, 11, true);
RowRunnerSelector s4s5 = RowRunnerSelector(0, 5, 7, 7, 100, 1, 17, true);
RowWipeSelector s4s6 = RowWipeSelector(0, 5, 0, 9, 4000, true, false);
StripStateStep State_4_steps[] = {
	{ 0,5000,&s4s1, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s2, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s3, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s4, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s5, 0, &OnNow, 0 ,0 },
	{ 1000,-1,&s4s6, 0, &s4e1, 0 ,&s4e2 },
};
StripState State_4 = StripState(&strip, State_4_steps, sizeof(State_4_steps) / sizeof(On[0]));

StripStateStep State_5_steps[] = {
	{ 0,5000,&s2s1, 0, 0,&s2e3,0 },
	{ 1000,3000,&s2s1, &s2e4, 0,0,0 },
	{ 4000,3000,&s2s2, &s2e4, 0,0,0 },
	{ 5000,3000,&s2s2, 0, 0, &s2e3, 0 }
};
StripState State_5 = StripState(&strip, State_5_steps, sizeof(State_5_steps) / sizeof(On[0]));



void setup(){
  Serial.begin(9600);
  for (byte i = 0; i < NUM_PINS; i++)
  {
    pinMode(pins[i], OUTPUT);
  }
  strip.begin();
}

void loop(){
  if (starting_up) {
    startup();
  }
  else
  {
    if (Serial.available()) {
      serial_char = Serial.read();
      if (selectedPin == NO_SELECTION) {
        selectThing(serial_char);
      } else {
        selectMode(serial_char);
      }
    }

    for (byte i = 0; i < NUM_PINS; i++)
    {
        PinState* s = pinStates[i];
        if (s != NULL)
        {
          s->update(pins[i]);
        }
    }
    if (stripState != NULL)
    {
      stripState->update();
    }
  }
}

void startup() {
  blink(BOT_1_PIN);
  blink(BOT_2_PIN);
  blink(BOT_3_PIN);
  blink(BOT_4_PIN);
  blink(TOP_1_PIN);
  blink(TOP_2_PIN);
  blink(TOP_3_PIN);
    
  starting_up = false;
}

void blink(byte pin)
{
  digitalWrite(pin, HIGH);
  delay(100);
  digitalWrite(pin, LOW);
}

void selectMode(char mode) {
  if (selectedPin < NUM_PINS) {
    selectButtonMode(mode);
  } else if (selectedPin == STRIP_STATE) {
    selectStripMode(mode);
  }

  selectedPin = NO_SELECTION;
}

void selectThing(char thing) {
  switch (thing) {
    case '1':
      selectedPin = 0;
      break;
    case '2':
      selectedPin = 1;
      break;
    case '3':
      selectedPin = 2;
      break;
    case '4':
      selectedPin = 3;
      break;
    case 'a':
      selectedPin = 4;
      break;
    case 'b':
      selectedPin = 5;
      break;
    case 'c':
      selectedPin = 6;
      break;
    case 's':
      selectedPin = STRIP_STATE;
      break;
  }
}

void selectStripMode(char mode) {
	StripState* oldState = stripState;
  switch (mode) {
    case 'f':
      Serial.println("Entering Off State");
      stripState = &OffState;
      break;
    case 'n':
      Serial.println("Entering On State");
      stripState = &OnState;
      break;
    case 'u':
		Serial.println("Entering Up State");
		stripState = &UpState;
      break;
    case '1':
		Serial.println("Entering 1 State");
		stripState = &State_1;

      break;
    case '2':
		Serial.println("Entering 2 State");
		stripState = &State_2;

      break;
    case '3':
		Serial.println("Entering 3 State");
		stripState = &State_3;

      break;
    case '4':
		Serial.println("Entering 4 State");
		stripState = &State_4;

      break;
	case '5':
		Serial.println("Entering 5 State");
		stripState = &State_5;

		break;
	case '6':

		break;
	case '7':

		break;
	case '8':

		break;
	case '9':

		break;
	case '0':

		break;
	case 'a':

		break;
	case 'b':

		break;
	case 'c':

		break;
	case 'd':

		break;
	case 'e':

		break;
  }
  stripState->reset(oldState);

}

void selectButtonMode(char mode) {
    switch (mode) {
      case 'n':
        pinStates[selectedPin] = &OnPin;
        break;
      case 'f':
        pinStates[selectedPin] = &OffPin;
        break;
      case 'b':
        pinStates[selectedPin] = &SlowBlink;
        break;
      case 's':
        pinStates[selectedPin] = &FastBlink;
        break;
    }
}
