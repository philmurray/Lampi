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
#include "RowWipeSelectorReverse.h"
#include "RowRunnerSelector.h"
#include "ColRunnerSelector.h"

#include <EEPROM.h>
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
#define STRIP_IDLE_STATE 8
#define NO_SELECTION 254

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_PINS, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);
char serial_char = 0;
bool starting_up = true;
byte selectedPin = NO_SELECTION;

byte pins[] = { BOT_1_PIN, BOT_2_PIN, BOT_3_PIN, BOT_4_PIN, TOP_1_PIN, TOP_2_PIN, TOP_3_PIN };
PinState* pinStates[7];

OnOffPinState OnPin = OnOffPinState(true);
OnOffPinState OffPin = OnOffPinState(false);
BlinkPinState SlowBlink = BlinkPinState(500, 250, 0);
BlinkPinState AltSlowBlink = BlinkPinState(500, 250, 250);
BlinkPinState FastBlink = BlinkPinState(200, 100, 0);


const int stripIdleAddr = 0;
char stripIdle = '0';
#define NUM_IDLE_STATES 11
char idleStates[] = { '0', '9', '3', '4', '5', '6', '7', '8', '1', '2', 'a' };


StripState stripState = StripState(&strip);


Selector AllSelector = Selector();

Ease OnNow = Ease(0, 255, 0, false);
Ease HalfNow = Ease(0, 120, 0, false);
Ease OffNow = Ease(0, 0, 0, false);
Ease NegNow = Ease(-255, -255, 0, false);
Ease MinNow = Ease(0, 10, 0, false);

LinearEase HalfSmallEase = LinearEase(0, 120, 300, false);
LinearEase OnSmallEase = LinearEase(0, 255, 300, false);

SineEase OffMediumEase = SineEase(0, 0, 750, false);
SineEase QuarterMediumEase = SineEase(0, 50, 750, false);
SineEase HalfMediumEase = SineEase(0, 120, 750, false);
SineEase OnMediumEase = SineEase(0, 255, 750, false);

LinearEase NegMediumSineEase = LinearEase(0, -255, 1000, false);
LinearEase HalfMediumSineEase = LinearEase(0, 120, 1000, false);
LinearEase OnMediumSineEase = LinearEase(0, 255, 1000, false);

LinearEase NegLongEase = LinearEase(0, -255, 3000, false);
LinearEase HalfLongEase = LinearEase(0, 120, 3000, false);
LinearEase OnLongEase = LinearEase(0, 255, 3000, false);

LinearEase MinVLongEase = LinearEase(0, 10, 4000, false);
LinearEase OnVLongEase = LinearEase(0, 200, 4000, false);


SineEase NegSmallSineWave = SineEase(0, -255, 250, true);
SineEase QuarterSmallSineWave = SineEase(0, 40, 250, true);
SineEase OnSmallSineWave = SineEase(0, 255, 250, true);
SineEase HalfMediumSineWave = SineEase(20, 120, 500, true);
LinearEase OnMediumSineWave = LinearEase(0, 255, 1000, true);
LinearEase OnLongSineWave = LinearEase(0, 255, 2000, true);
LinearEase OnSuperLongSineWave = LinearEase(0, 255, 8000, true);


const PROGMEM StripStateStep On[] = {
  {0,-1,&AllSelector, 0, 0, 0, &OnNow}
};

const PROGMEM StripStateStep Off[] = {
  {0,-1,&AllSelector, 0, 0, 0, &OffNow }
};


RowRunnerSelector UpR = RowRunnerSelector(0, 5, 0, 9, 60, 3, 6, false);
const PROGMEM StripStateStep Up[] = {
	{0,-1,&UpR, 0,0,0,&OnNow}
};

const PROGMEM StripStateStep State_1_steps[] = {
	{ 0, -1, &AllSelector, 0, 0, &MinNow, &MinNow },
	{ 500, 1000, &AllSelector, 0, 0, &OnSmallSineWave, 0 },
	{ 2250, 1000, &AllSelector, 0, 0, &OnSmallSineWave, 0 },
	{ 4000, 1000, &AllSelector, 0, 0, &OnSmallSineWave, 0 }
};

RowWipeSelector s2s1 = RowWipeSelector(0, 5, 0, 9, 2000, true);
RowWipeSelectorReverse s2s2 = RowWipeSelectorReverse(0, 5, 0, 9, 2000, true, 1000);
const PROGMEM StripStateStep State_2_steps[] = {
	{ 0,3000,&s2s1, &HalfMediumSineEase, 0,&OnMediumSineEase,0 },
	{ 3000,6000,&s2s2, &HalfMediumSineEase, 0, &OnMediumSineEase, 0 }
};

RowWipeSelector s3s1 = RowWipeSelector(0, 5, 4, 4, 2000, true);
ColWipeSelector s3s2 = ColWipeSelector(0, 5, 0, 4, 2000, false);
ColWipeSelector s3s3 = ColWipeSelector(0, 5, 4, 9, 2000, true);
const PROGMEM StripStateStep State_3_steps[] = {
	{ 0,-1,&s3s1, &HalfMediumEase, &OffMediumEase, &QuarterMediumEase,0 },
	{ 2000,-1,&s3s2, &OnMediumEase, 0,0,0 },
	{ 2000,-1,&s3s3, &OnMediumEase, 0,0,0 }
};

RowRunnerSelector s4s1 = RowRunnerSelector(0, 5, 5, 5, 100, 1, 7, true);
RowRunnerSelector s4s2 = RowRunnerSelector(0, 5, 3, 3, 100, 1, 13, true);
RowRunnerSelector s4s3 = RowRunnerSelector(0, 5, 4, 4, 100, 1, 19, true);
RowRunnerSelector s4s4 = RowRunnerSelector(0, 5, 1, 1, 100, 1, 11, true);
RowRunnerSelector s4s5 = RowRunnerSelector(0, 5, 7, 7, 100, 1, 17, true);
RowWipeSelector s4s6 = RowWipeSelector(0, 5, 0, 9, 4000, true);
const PROGMEM StripStateStep State_4_steps[] = {
	{ 0,5000,&s4s1, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s2, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s3, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s4, 0, &OnNow, 0 ,0 },
	{ 0,5000,&s4s5, 0, &OnNow, 0 ,0 },
	{ 1000,-1,&s4s6, 0, &OnVLongEase, 0 ,&MinVLongEase },
};

const PROGMEM StripStateStep State_5_steps[] = {
	{ 0,5000,&s2s1, 0, 0,&OnMediumSineEase,0 },
	{ 1000,3000,&s2s1, &HalfMediumSineEase, 0,0,0 },
	{ 4000,3000,&s2s2, &HalfMediumSineEase, 0,0,0 },
	{ 5000,3000,&s2s2, 0, 0, &OnMediumSineEase, 0 }
};

ColWipeSelector s6s1 = ColWipeSelector(0, 1, 0, 9, 750, true);
ColWipeSelector s6s2 = ColWipeSelector(2, 3, 0, 9, 750, false);
ColWipeSelector s6s3 = ColWipeSelector(4, 5, 0, 9, 750, true);
BlockSelector s6s4 = BlockSelector(1, 4, 3, 6);
const PROGMEM StripStateStep State_6_steps[] = {
	{ 0, -1, &s6s1, &OnMediumEase, 0,0,0 },
	{ 500, -1, &s6s2, &OnMediumEase, 0,0,0 },
	{ 1000, -1, &s6s3, &OnMediumEase, 0,0,0 },
	{ 2250, 1000, &s6s4, &NegSmallSineWave, 0, &OnSmallSineWave, 0 },
	{ 4000, 1000, &s6s4, &NegSmallSineWave, 0, &OnSmallSineWave, 0 }
};

RowWipeSelector s7s1 = RowWipeSelector(0, 5, 0, 2, 2500, true);
RowWipeSelector s7s2 = RowWipeSelector(0, 5, 7, 9, 2500, false);
ColWipeSelector s7s3 = ColWipeSelector(0, 5, 2, 9, 5000, true);
ColWipeSelector s7s4 = ColWipeSelector(0, 5, 0, 7, 5000, false);
const PROGMEM StripStateStep State_7_steps[] = {
	{ 0, -1, &s7s1, 0,&OnMediumEase,0, 0 },
	{ 0, -1, &s7s2, 0, 0, &OnMediumEase, 0 },
	{ 2000, -1, &s7s3, 0,&OnMediumEase,0, 0 },
	{ 2000, -1, &s7s4, 0, 0, &OnMediumEase, 0 }
};

RowRunnerSelector s8s1 = RowRunnerSelector(0, 5, 2, 3, 100, 6, 13, false);
RowRunnerSelector s8s2 = RowRunnerSelector(0, 5, 4, 5, 100, 6, 13, false);
RowRunnerSelector s8s3 = RowRunnerSelector(0, 5, 6, 7, 100, 6, 13, false);
const PROGMEM StripStateStep State_8_steps[] = {
	{ 0, -1, &AllSelector, &HalfNow, 0, &OnNow, 0 },
	{ 0, -1, &s8s1, &OnMediumSineEase, 0, &NegMediumSineEase, 0 },
	{ 400, -1, &s8s2, &OnMediumSineEase, 0, &NegMediumSineEase, 0 },
	{ 800, -1, &s8s3, &OnMediumSineEase, 0,&NegMediumSineEase, 0 },
};

BlockSelector s9s1 = BlockSelector(2, 2, 4, 4);
BlockSelector s9s2 = BlockSelector(3, 3, 6, 6);
BlockSelector s9s3 = BlockSelector(5, 5, 5, 5);
BlockSelector s9s4 = BlockSelector(2, 2, 8, 8);
BlockSelector s9s5 = BlockSelector(5, 5, 2, 2);
BlockSelector s9s6 = BlockSelector(4, 4, 4, 4);
BlockSelector s9s7 = BlockSelector(4, 4, 7, 7);
BlockSelector s9s8 = BlockSelector(3, 3, 3, 3);
const PROGMEM StripStateStep State_9_steps[] = {
	{ 0, 250, &AllSelector, &HalfNow, 0, &OnNow, 0},
	{ 250, -1, &AllSelector, 0, &OnNow, 0, 0 },
	{ 250, -1, &s2s2, &HalfMediumSineEase, &NegMediumSineEase, &OnMediumSineEase, 0 },
	{ 2750, -1, &s9s1, &HalfMediumSineEase, &NegNow, &OnMediumSineEase, &QuarterSmallSineWave },
	{ 2900, -1, &s9s2, &HalfMediumSineEase, &NegNow, &OnMediumSineEase, &QuarterSmallSineWave },
	{ 3050, -1, &s9s3, &HalfMediumSineEase, &NegNow, &OnMediumSineEase, &QuarterSmallSineWave },
	{ 3200, -1, &s9s4, &HalfMediumSineEase, &NegNow, &OnMediumSineEase, &QuarterSmallSineWave },
	{ 3350, -1, &s9s5, &HalfMediumSineEase, &NegNow, &OnMediumSineEase, &QuarterSmallSineWave },
	{ 3500, -1, &s9s6, &HalfMediumSineEase, &NegNow, &OnMediumSineEase, &QuarterSmallSineWave },
	{ 3650, -1, &s9s7, &HalfMediumSineEase, &NegNow, &OnMediumSineEase, &QuarterSmallSineWave },
	{ 3800, -1, &s9s8, &HalfMediumSineEase, &NegNow, &OnMediumSineEase, &QuarterSmallSineWave }
};


RowWipeSelector s10s1 = RowWipeSelector(3, 5, 0, 9, 2000, false);
const PROGMEM StripStateStep State_10_steps[] = {
	{ 0,-1,&s4s1, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s2, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s3, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s4, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s5, 0, &OnNow, 0 ,0 },
	{ 1000,-1,&s3s1, &OnMediumEase, 0, 0,0 },
	{ 3000,-1,&s10s1, &OnMediumEase, 0, 0,0 }
};
RowWipeSelector sas1 = RowWipeSelector(0, 5, 0, 9, 2000, true);
ColWipeSelector sas2 = ColWipeSelector(0, 5, 0, 9, 2000, true);
const PROGMEM StripStateStep State_a_steps[] = {
	{ 0,-1,&sas1, 0, 0, &OnLongSineWave ,0 },
	{ 0,-1,&sas2, &OnLongSineWave, 0, 0 ,0 }
};


BlockSelector sbs1 = BlockSelector(0, 0, 0, 9);
RowRunnerSelector sbs2 = RowRunnerSelector(1, 5, 0, 9, 150, 2, 3, false);
RowRunnerSelector sbs3 = RowRunnerSelector(1, 3, 0, 9, 250, 3, 4, false);

const PROGMEM StripStateStep State_b_steps[] = {
	{0,-1, &sbs1, 0, &HalfMediumSineWave, 0, 0 },
	{ 0,-1, &sbs2, 0, 0, &HalfSmallEase, 0 },
	{ 0,-1, &sbs3, &HalfSmallEase, 0, &OnSmallEase, 0 }
};

ColWipeSelector scs1 = ColWipeSelector(0, 1, 0, 9, 750, true);
ColWipeSelector scs2 = ColWipeSelector(2, 3, 0, 9, 750, false);
ColWipeSelector scs3 = ColWipeSelector(4, 5, 0, 9, 750, true);
RowWipeSelector scs4 = RowWipeSelector(0, 5, 0, 2, 750, true);
RowWipeSelector scs5 = RowWipeSelector(0, 5, 3, 6, 750, false);
RowWipeSelector scs6 = RowWipeSelector(0, 5, 7, 9, 750, true);
const PROGMEM StripStateStep State_c_steps[] = {
	{ 0,-1, &scs1, &OnMediumSineWave, 0, 0, 0 },
	{ 750,-1, &scs2, 0, 0, &OnMediumSineWave, 0},
	{ 1500,-1, &scs3, 0, &OnMediumSineWave, 0, 0 },
	//{ 2500,-1, &scs4, &s3e2, 0, 0, 0 },
	//{ 3250,-1, &scs5, 0, 0, &s3e2, 0 },
	//{ 4000,-1, &scs6, 0, &s3e2, 0, 0 },

};

RowWipeSelector sds1 = RowWipeSelector(0, 1, 0, 9, 1000, false);
const PROGMEM StripStateStep State_d_steps[] = {
	{ 0,-1,&s4s1, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s2, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s3, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s4, 0, &OnNow, 0 ,0 },
	{ 0,-1,&s4s5, 0, &OnNow, 0 ,0 },
	{ 2000, -1, &AllSelector, &HalfLongEase, &NegLongEase, &OnLongEase, 0},
	{ 3000, -1, &sds1, &OnMediumEase, 0, &NegMediumSineEase, 0 }
};


ColWipeSelector ses1 = ColWipeSelector(0, 5, 0, 9, 2000, true);
const PROGMEM StripStateStep State_e_steps[] = {
	{ 0, -1, &ses1, &OnMediumSineWave, 0, 0, 0 },
	{ 666, -1, &ses1, 0, &OnMediumSineWave, 0, 0 },
	{ 1333, -1, &ses1, 0, 0, &OnMediumSineWave, 0 }
};

const PROGMEM StripStateStep Idle_0[] = {
	{ 0, -1, &AllSelector, 0, 0, 0, &MinNow }
};

const PROGMEM StripStateStep Idle_1[] = {
	{ 0, -1, &AllSelector, 0, 0, 0, &MinNow },
	{ 0, -1, &AllSelector, &OnSuperLongSineWave, 0, 0, 0 },
	{ 5333, -1, &AllSelector, 0, &OnSuperLongSineWave, 0, 0 },
	{ 10666, -1, &AllSelector, 0, 0, &OnSuperLongSineWave, 0 }
};


ColRunnerSelector i2s1 = ColRunnerSelector(0, 5, 0, 9, 1000, 3, 6, false);
RowRunnerSelector i2s2 = RowRunnerSelector(0, 5, 0, 9, 1000, 3, 6, false);
const PROGMEM StripStateStep Idle_2[] = {
	{ 0, -1, &AllSelector, 0, 0, 0, &MinNow },
	{ 0, -1, &i2s1, &OnMediumSineEase, 0, 0, 0 },
	{ 250, -1, &i2s2, 0, &OnMediumSineEase, 0, 0 },
	{ 0, -1, &AllSelector, 0, 0, &OnSuperLongSineWave, 0 }
};

const PROGMEM StripStateStep Idle_3[] = {
	{ 0, -1, &AllSelector, &HalfNow, 0, 0, &MinNow }
};
const PROGMEM StripStateStep Idle_4[] = {
	{ 0, -1, &AllSelector, 0, &HalfNow, 0, &MinNow }
};
const PROGMEM StripStateStep Idle_5[] = {
	{ 0, -1, &AllSelector, 0, 0, &HalfNow, &MinNow }
};
const PROGMEM StripStateStep Idle_6[] = {
	{ 0, -1, &AllSelector, &HalfNow, 0, &HalfNow, &MinNow }
};
const PROGMEM StripStateStep Idle_7[] = {
	{ 0, -1, &AllSelector, &HalfNow, &HalfNow, 0, &MinNow }
};
const PROGMEM StripStateStep Idle_8[] = {
	{ 0, -1, &AllSelector, 0, &HalfNow, &HalfNow, &MinNow }
};

ColRunnerSelector ias1 = ColRunnerSelector(2, 2, 0, 9, 100, 4, 30, false);
ColRunnerSelector ias2 = ColRunnerSelector(3, 3, 0, 9, 100, 4, 31, false);
ColRunnerSelector ias3 = ColRunnerSelector(4, 4, 0, 9, 100, 4, 32, false);
ColRunnerSelector ias4 = ColRunnerSelector(1, 1, 0, 9, 100, 4, 33, false);
ColRunnerSelector ias5 = ColRunnerSelector(0, 0, 0, 9, 100, 4, 34, false);
ColRunnerSelector ias6 = ColRunnerSelector(5, 5, 0, 9, 100, 4, 35, false);
const PROGMEM StripStateStep Idle_9[] = {
	{ 0, -1, &AllSelector, 0, 0, 0, &MinNow },
	{ 750, -1, &ias1, &HalfSmallEase, &HalfSmallEase, 0, 0 },
	{ 1500, -1, &ias2, &OnSmallEase, 0, 0, 0 },
	{ 2000, -1, &ias3, 0, &OnSmallEase, 0, 0 },
	{ 2250, -1, &ias4, 0, 0, &OnSmallEase, 0 },
	{ 250, -1, &ias5, &HalfSmallEase, 0, &HalfSmallEase, 0 },
	{ 1000, -1, &ias6, 0, &HalfSmallEase, &HalfSmallEase, 0 }
};

void setup() {
	Serial.begin(9600);
	for (byte i = 0; i < NUM_PINS; i++)
	{
		pinMode(pins[i], OUTPUT);
	}

	EEPROM.get(stripIdleAddr, stripIdle);
	Serial.print(F("Got idle val from eeprom "));
	Serial.println(stripIdle);

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

	selectThing('1');
	selectMode('b');
	showStripIdle(1200);
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
	else if (selectedPin == STRIP_IDLE_STATE) {
		selectStripIdleMode(mode);
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
	case 'z':
		selectedPin = STRIP_IDLE_STATE;
		break;
	}
}
void selectStripIdleMode(char mode) {


	if (mode == '-' || mode == '+')
	{
		int index = 0;
		for (int i = 0; i < NUM_IDLE_STATES; i++)
		{
			if (idleStates[i] == stripIdle)
			{
				index = i;
				break;
			}
		}
		if (mode == '-')
		{
			stripIdle = idleStates[(index + NUM_IDLE_STATES - 1) % NUM_IDLE_STATES];
		}
		else
		{
			stripIdle = idleStates[(index + NUM_IDLE_STATES + 1) % NUM_IDLE_STATES];
		}
	}
	else
	{
		stripIdle = mode;
	}


	EEPROM.put(stripIdleAddr, stripIdle);

	Serial.print(F("Setting Idle to "));
	Serial.println(mode);

	showStripIdle(250);
}

void showStripIdle(int fade) {
	switch (stripIdle)
	{
		case '0':
			stripState.reset(On, sizeof(On) / sizeof(On[0]), fade);
			break;
		case '1':
			stripState.reset(Idle_1, sizeof(Idle_1) / sizeof(On[0]), fade);
			break;
		case '2':
			stripState.reset(Idle_2, sizeof(Idle_2) / sizeof(On[0]), fade);
			break;
		case '3':
			stripState.reset(Idle_3, sizeof(Idle_3) / sizeof(On[0]), fade);
			break;
		case '4':
			stripState.reset(Idle_4, sizeof(Idle_4) / sizeof(On[0]), fade);
			break;
		case '5':
			stripState.reset(Idle_5, sizeof(Idle_5) / sizeof(On[0]), fade);
			break;
		case '6':
			stripState.reset(Idle_6, sizeof(Idle_6) / sizeof(On[0]), fade);
			break;
		case '7':
			stripState.reset(Idle_7, sizeof(Idle_7) / sizeof(On[0]), fade);
			break;
		case '8':
			stripState.reset(Idle_8, sizeof(Idle_8) / sizeof(On[0]), fade);
			break;
		case '9':
			stripState.reset(Idle_0, sizeof(Idle_0) / sizeof(On[0]), fade);
			break;
		case 'a':
			stripState.reset(Idle_9, sizeof(Idle_9) / sizeof(On[0]), fade);
			break;
		default:
			stripState.reset(On, sizeof(On) / sizeof(On[0]), fade);
	}
}

void selectStripMode(char mode) {
	switch (mode) {
	case 'f':
		Serial.println(F("Entering Off State"));
		stripState.reset(Off, sizeof(Off) / sizeof(On[0]), 1200);
		break;
	case 'n':
		showStripIdle(1200);
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
		Serial.println(F("Entering 9 State"));
		stripState.reset(State_10_steps, sizeof(State_10_steps) / sizeof(On[0]), 250);

		break;
	case 'a':
		Serial.println(F("Entering a State"));
		stripState.reset(State_a_steps, sizeof(State_a_steps) / sizeof(On[0]), 250);

		break;
	case 'b':
		Serial.println(F("Entering b State"));
		stripState.reset(State_b_steps, sizeof(State_b_steps) / sizeof(On[0]), 250);

		break;
	case 'c':
		Serial.println(F("Entering c State"));
		stripState.reset(State_c_steps, sizeof(State_c_steps) / sizeof(On[0]), 250);

		break;
	case 'd':
		Serial.println(F("Entering d State"));
		stripState.reset(State_d_steps, sizeof(State_d_steps) / sizeof(On[0]), 250);

		break;
	case 'e':
		Serial.println(F("Entering e State"));
		stripState.reset(State_e_steps, sizeof(State_e_steps) / sizeof(On[0]), 250);

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
	case 'c':
		pinStates[selectedPin] = &AltSlowBlink;
		break;
	case 's':
		pinStates[selectedPin] = &FastBlink;
		break;
	}
}
