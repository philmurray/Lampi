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

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_PINS, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);
char serial_char = 0;
bool starting_up = true;
int selectedPin = -1;

int pins[] = {BOT_1_PIN, BOT_2_PIN, BOT_3_PIN, BOT_4_PIN, TOP_1_PIN, TOP_2_PIN, TOP_3_PIN};
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


LinearEase s1e1 = LinearEase(0, 2000, 6, false);
LinearEase s1e2 = LinearEase(0, 2000, 5, false);
LinearEase s1e3 = LinearEase(2000, 0, 5, false);

SineEase s1e4 = SineEase(0, 255, 750, false);
RowWipeSelector s1s1 = RowWipeSelector(0, 5, 4, 5, &s1e1, false);
ColWipeSelector s1s2 = ColWipeSelector(3, 5, 0, 4, &s1e3, false);
ColWipeSelector s1s3 = ColWipeSelector(3, 5, 5, 9, &s1e2, false);

StripStateStep State_1_steps[] = {
	{ 0,-1,&s1s1, &s1e4, 0,0,0 },
	{ 2000,-1,&s1s2, &s1e4, 0,0,0 },
	{ 2000,-1,&s1s3, &s1e4, 0,0,0 }
};
StripState State_1 = StripState(&strip, State_1_steps, sizeof(State_1_steps) / sizeof(On[0]));

LinearEase s2e1 = LinearEase(0, 3000, 6, false);
LinearEase s2e3 = LinearEase(0, 255, 3000, false);
LinearEase s2e4 = LinearEase(0, 200, 3000, false);
RowWipeSelector s2s1 = RowWipeSelector(0, 5, 0, 9, &s2e1, false);
RowWipeSelector s2s2 = RowWipeSelector(0, 5, 0, 9, &s2e1, true);
StripStateStep State_2_steps[] = {
	{ 0,3000,&s2s1, &s2e4, 0,&s2e3,0 },
	{ 3000,3000,&s2s2, &s2e4, 0, &s2e3, 0}
};
StripState State_2 = StripState(&strip, State_2_steps, sizeof(State_2_steps) / sizeof(On[0]));

//
//StripStateStep Test[]  = {
//  {0, -1, &runner, NULL, NULL, NULL, &lEaseOnHalf },
//  //{0, -1, &runner2, NULL, NULL, &lEaseOnHalf, NULL },
//  //{ 0, -1, &WipeUp, &EaseInOut, NULL, NULL, NULL },
//  //{ 0, -1, &WipeOver, NULL, &EaseInOut, NULL, NULL }
//  //{0,-1,new BlockSelector(0,0,0,0), NULL, NULL, NULL, &lEaseOnHalf}
//  //{0,2000, &Col123, NULL, NULL, NULL, &lEaseOnHalf},
//  //{2000,4000, &Col123, NULL, NULL, NULL, &lEaseOffHalf},
//  //{500,2500,&Col456, &lEaseOnHalf, NULL, NULL, NULL},
//  //{2500,4500,&Col456, &lEaseOffHalf, NULL, NULL, NULL},
//  //{1000,3000,&Col78, NULL, &lEaseOnHalf, NULL, NULL},
//  //{3000,5000,&Col78, NULL, &lEaseOffHalf, NULL, NULL},
//  //{1500,3500,&Col910, NULL, NULL, &lEaseOnHalf, NULL},
//  //{3500,5500,&Col910, NULL, NULL, &lEaseOffHalf, NULL},
//  //{0,9000,&Row12, NULL, &lEaseOnHalf, &lEaseOnHalf, NULL},
//  //{1500,9000,&Row34, &lEaseOnHalf, NULL, &lEaseOnHalf, NULL},
//  //{3000,9000,&Row56, &lEaseOnHalf, &lEaseOnHalf, NULL, NULL},
//};
//StripStateStep Test2[] = {
//	//{ 0, -1, &runner, NULL, NULL, NULL, &lEaseOnHalf },
//	{ 0, -1, &runner2, NULL, NULL, &lEaseOnHalf, NULL },
//	//{ 0, -1, &WipeUp, &EaseInOut, NULL, NULL, NULL },
//	//{ 0, -1, &WipeOver, NULL, &EaseInOut, NULL, NULL }
//	//{0,-1,new BlockSelector(0,0,0,0), NULL, NULL, NULL, &lEaseOnHalf}
//	//{0,2000, &Col123, NULL, NULL, NULL, &lEaseOnHalf},
//	//{2000,4000, &Col123, NULL, NULL, NULL, &lEaseOffHalf},
//	//{500,2500,&Col456, &lEaseOnHalf, NULL, NULL, NULL},
//	//{2500,4500,&Col456, &lEaseOffHalf, NULL, NULL, NULL},
//	//{1000,3000,&Col78, NULL, &lEaseOnHalf, NULL, NULL},
//	//{3000,5000,&Col78, NULL, &lEaseOffHalf, NULL, NULL},
//	//{1500,3500,&Col910, NULL, NULL, &lEaseOnHalf, NULL},
//	//{3500,5500,&Col910, NULL, NULL, &lEaseOffHalf, NULL},
//	//{0,9000,&Row12, NULL, &lEaseOnHalf, &lEaseOnHalf, NULL},
//	//{1500,9000,&Row34, &lEaseOnHalf, NULL, &lEaseOnHalf, NULL},
//	//{3000,9000,&Row56, &lEaseOnHalf, &lEaseOnHalf, NULL, NULL},
//};

//BlockSelector Col123 = BlockSelector(0,5,0,2);
//BlockSelector Col456 = BlockSelector(0,5,3,5);
//BlockSelector Col78 = BlockSelector(0,5,6,7);
//BlockSelector Col910 = BlockSelector(0,5,8,9);
//BlockSelector Row12 = BlockSelector(0,1,0,9);
//BlockSelector Row34 = BlockSelector(2,3,0,9);
//BlockSelector Row56 = BlockSelector(4,5,0,9);
//
//LinearEase WipeUpEase = LinearEase(0, 1000, 4, false);
//RowWipeSelector WipeUp = RowWipeSelector(0, 5, 0, 9, &WipeUpEase);
//
//LinearEase WipeOverEase = LinearEase(0, 1000, 9, false);
//ColWipeSelector WipeOver = ColWipeSelector(0, 5, 0, 9, &WipeOverEase);
//
//RowRunnerSelector runner = RowRunnerSelector(0, 5, 0, 9, 1000, 2, 7, true);
//ColRunnerSelector runner2 = ColRunnerSelector(0, 5, 0, 9, 1000, 2, 5, true);
//
//SineEase EaseOn = SineEase(0,255,2000, false);

//SineEase lEaseOff = SineEase(255,0,1000, false);
//LinearEase lEaseOnHalf = LinearEase(0,75,1000, false);
//LinearEase lEaseOffHalf = LinearEase(75,0,1000, false);
//
//SineEase EaseInOut = SineEase(10, 75, 1000, true);
//LinearEase LinearEaseInOut = LinearEase(0, 255, 1000, true);


void setup(){
  Serial.begin(9600);
  for (int i = 0; i < NUM_PINS; i++)
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
      if (selectedPin == -1) {
        selectThing(serial_char);
      } else {
        selectMode(serial_char);
      }
    }

    for (int i = 0; i < NUM_PINS; i++)
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

void blink(int pin)
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

  selectedPin = -1;
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
		Serial.println("Entering 1 State");
		stripState = &State_2;

      break;
    case '3':

      break;
    case '4':

      break;
	case '5':

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
