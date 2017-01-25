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

byte pins[] = { BOT_1_PIN, BOT_2_PIN, BOT_3_PIN, BOT_4_PIN, TOP_1_PIN, TOP_2_PIN, TOP_3_PIN };
PinState* pinStates[7];

OnOffPinState OnPin = OnOffPinState(true);
OnOffPinState OffPin = OnOffPinState(false);
BlinkPinState SlowBlink = BlinkPinState(500, 250);
BlinkPinState FastBlink = BlinkPinState(200, 100);

StripState stripState = StripState(&strip);


Selector AllSelector = Selector();

Ease OnNow = Ease(0, 255, 0, false);
Ease OffNow = Ease(0, 0, 0, false);
Ease SubtractNow = Ease(-255, -255, 0, false);
SineEase OnMediumEase = SineEase(0, 255, 750, false);
SineEase OffMediumEase = SineEase(0, 0, 750, false);


StripStateStep On[] = {
  {0,-1,&AllSelector, 0, 0, 0, &OnNow}
};

StripStateStep Off[] = {
  {0,-1,&AllSelector, 0, 0, 0, &OffNow }
};


RowRunnerSelector UpR = RowRunnerSelector(0, 5, 0, 9, 60, 3, 6, false);
StripStateStep Up[] = {
	{0,-1,&UpR, 0,0,0,&OnNow}
};

Ease s1e1 = Ease(0, 10, 0, false);
SineEase s1e2 = SineEase(0, 255, 250, true);
StripStateStep State_1_steps[] = {
	{ 0, -1, &AllSelector, 0, 0, &s1e1, &s1e1 },
	{ 500, 1000, &AllSelector, 0, 0, &s1e2, 0 },
	{ 2250, 1000, &AllSelector, 0, 0, &s1e2, 0 },
	{ 4000, 1000, &AllSelector, 0, 0, &s1e2, 0 }
};

LinearEase s2e3 = LinearEase(0, 255, 1000, false);
LinearEase s2e4 = LinearEase(0, 120, 1000, false);
RowWipeSelector s2s1 = RowWipeSelector(0, 5, 0, 9, 2000, true, false, 0);
RowWipeSelector s2s2 = RowWipeSelector(0, 5, 0, 9, 2000, true, true, 1000);
StripStateStep State_2_steps[] = {
	{ 0,3000,&s2s1, &s2e4, 0,&s2e3,0 },
	{ 3000,6000,&s2s2, &s2e4, 0, &s2e3, 0 }
};

SineEase s3e2 = SineEase(0, 120, 750, false);
SineEase s3e4 = SineEase(0, 50, 750, false);
RowWipeSelector s3s1 = RowWipeSelector(0, 5, 4, 4, 2000, true, false, 0);
ColWipeSelector s3s2 = ColWipeSelector(0, 5, 0, 4, 2000, false, false, 0);
ColWipeSelector s3s3 = ColWipeSelector(0, 5, 4, 9, 2000, true, false, 0);
StripStateStep State_3_steps[] = {
	{ 0,-1,&s3s1, &s3e2, &OffMediumEase, &s3e4,0 },
	{ 2000,-1,&s3s2, &OnMediumEase, 0,0,0 },
	{ 2000,-1,&s3s3, &OnMediumEase, 0,0,0 }
};

LinearEase s4e1 = LinearEase(0, 200, 4000, false);
LinearEase s4e2 = LinearEase(0, 10, 4000, false);
RowRunnerSelector s4s1 = RowRunnerSelector(0, 5, 5, 5, 100, 1, 7, true);
RowRunnerSelector s4s2 = RowRunnerSelector(0, 5, 3, 3, 100, 1, 13, true);
RowRunnerSelector s4s3 = RowRunnerSelector(0, 5, 4, 4, 100, 1, 19, true);
RowRunnerSelector s4s4 = RowRunnerSelector(0, 5, 1, 1, 100, 1, 11, true);
RowRunnerSelector s4s5 = RowRunnerSelector(0, 5, 7, 7, 100, 1, 17, true);
RowWipeSelector s4s6 = RowWipeSelector(0, 5, 0, 9, 4000, true, false, 0);
StripStateStep State_4_steps[] = {
	{ 0,5000,&s4s1, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s2, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s3, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s4, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s5, 0, &OnNow, 0 ,0 },
	{ 1000,-1,&s4s6, 0, &s4e1, 0 ,&s4e2 },
};

StripStateStep State_5_steps[] = {
	{ 0,5000,&s2s1, 0, 0,&s2e3,0 },
	{ 1000,3000,&s2s1, &s2e4, 0,0,0 },
	{ 4000,3000,&s2s2, &s2e4, 0,0,0 },
	{ 5000,3000,&s2s2, 0, 0, &s2e3, 0 }
};

ColRunnerSelector s6s1 = ColRunnerSelector(0, 1, 0, 9, 250, 2, 5, false);
ColRunnerSelector s6s2 = ColRunnerSelector(2, 3, 0, 9, 250, 2, 5, true);
ColRunnerSelector s6s3 = ColRunnerSelector(4, 5, 0, 9, 250, 2, 5, false);
StripStateStep State_6_steps[] = {
	{ 0, -1, &AllSelector, 0,0,&OnNow, 0},
	{ 0, -1, &s6s1, &OnNow, 0,&SubtractNow,0 },
	{ 0, -1, &s6s2, &OnNow, 0,&SubtractNow,0 },
	{ 0, -1, &s6s3, &OnNow, 0,&SubtractNow,0 }
};

RowWipeSelector s7s1 = RowWipeSelector(0,5,0,2,2500,true,false,0);
RowWipeSelector s7s2 = RowWipeSelector(0, 5, 7, 9, 2500, false, false, 0);
ColWipeSelector s7s3 = ColWipeSelector(0, 5, 2, 9, 5000, true, false, 0);
ColWipeSelector s7s4 = ColWipeSelector(0, 5, 0, 7, 5000, false, false, 0);
StripStateStep State_7_steps[] = {
	{ 0, -1, &s7s1, 0,&OnMediumEase,0, 0 },
	{ 0, -1, &s7s2, 0, 0, &OnMediumEase, 0 },
	{ 2500, -1, &s7s3, 0,&OnMediumEase,0, 0 },
	{ 2500, -1, &s7s4, 0, 0, &OnMediumEase, 0 }
};

ColRunnerSelector s8s1 = ColRunnerSelector(0, 5, 0, 9, 150, 5, 10, false);
StripStateStep State_8_steps[] = {
	{ 0, -1, &AllSelector, &s2e4,0,0, 0 },
	{ 0, -1, &s8s1, 0,0,&OnMediumEase , 0 }
};


RowWipeSelector s9s1 = RowWipeSelector(4, 5, 0, 9, 2000, false, false, 0);
StripStateStep State_9_steps[] = {
	{ 0,-1,&s4s1, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s2, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s3, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s4, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s5, 0, &OnNow, 0 ,0 },
	{ 1000,-1,&s3s1, &OnMediumEase, 0, 0,0 },
	{ 3000,-1,&s9s1, &OnMediumEase, 0, 0,0 }
};


void setup() {
	Serial.begin(9600);
	for (byte i = 0; i < NUM_PINS; i++)
	{
		pinMode(pins[i], OUTPUT);
	}
	strip.begin();
}

void loop() {
	if (starting_up) {
		startup();
	}
	else
	{
		if (Serial.available()) {
			serial_char = Serial.read();
			if (selectedPin == NO_SELECTION) {
				selectThing(serial_char);
			}
			else {
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
		stripState.update();
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
	}
	else if (selectedPin == STRIP_STATE) {
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
	switch (mode) {
	case 'f':
		Serial.println(F("Entering Off State"));
		stripState.reset(Off, sizeof(Off) / sizeof(On[0]), 1200);
		break;
	case 'n':
		Serial.println(F("Entering On State"));
		stripState.reset(On, sizeof(On) / sizeof(On[0]), 1200);
		break;
	case 'u':
		Serial.println(F("Entering Up State"));
		stripState.reset(Up, sizeof(Up) / sizeof(On[0]), 0.0f);
		break;
	case '1':
		Serial.println(F("Entering 1 State"));
		stripState.reset(State_1_steps, sizeof(State_1_steps) / sizeof(On[0]), 250);

		break;
	case '2':
		Serial.println(F("Entering 2 State"));
		stripState.reset(State_2_steps, sizeof(State_2_steps) / sizeof(On[0]), 250);

		break;
	case '3':
		Serial.println(F("Entering 3 State"));
		stripState.reset(State_3_steps, sizeof(State_3_steps) / sizeof(On[0]), 250);

		break;
	case '4':
		Serial.println(F("Entering 4 State"));
		stripState.reset(State_4_steps, sizeof(State_4_steps) / sizeof(On[0]), 250);

		break;
	case '5':
		Serial.println(F("Entering 5 State"));
		stripState.reset(State_5_steps, sizeof(State_5_steps) / sizeof(On[0]), 250);

		break;
	case '6':
		Serial.println(F("Entering 6 State"));
		stripState.reset(State_6_steps, sizeof(State_6_steps) / sizeof(On[0]), 250);

		break;
	case '7':
		Serial.println(F("Entering 7 State"));
		stripState.reset(State_7_steps, sizeof(State_7_steps) / sizeof(On[0]), 250);

		break;
	case '8':
		Serial.println(F("Entering 8 State"));
		stripState.reset(State_8_steps, sizeof(State_8_steps) / sizeof(On[0]), 250);

		break;
	case '9':
		Serial.println(F("Entering 9 State"));
		stripState.reset(State_9_steps, sizeof(State_9_steps) / sizeof(On[0]), 250);

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
