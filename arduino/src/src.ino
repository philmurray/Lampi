#include "State.h"
#include "PinState.h"
#include "OnOffPinState.h"
#include "BlinkPinState.h"
#include "StripState.h"
#include "StripStateStep.h"

#include "Ease.h"
#include "LinearEase.h"

#include "Selector.h"
#include "BlockSelector.h"
#include "RowWipeSelector.h"

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
#define NUM_STATES 8
#define STRIP_STATE 7

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_PINS, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);
char serial_char = 0;
bool starting_up = true;
int selectedState = -1;
int selectedPin = -1;

State* States[8];

Selector AllSelector = Selector();

BlockSelector Col123 = BlockSelector(0,5,0,2);
BlockSelector Col456 = BlockSelector(0,5,3,5);
BlockSelector Col78 = BlockSelector(0,5,6,7);
BlockSelector Col910 = BlockSelector(0,5,8,9);
BlockSelector Row12 = BlockSelector(0,1,0,9);
BlockSelector Row34 = BlockSelector(2,3,0,9);
BlockSelector Row56 = BlockSelector(4,5,0,9);

LinearEase lEaseOn = LinearEase(0,255,1000, false);
LinearEase lEaseOff = LinearEase(255,0,1000, false);
LinearEase lEaseOnHalf = LinearEase(0,75,1000, false);
LinearEase lEaseOffHalf = LinearEase(75,0,1000, false);

StripStateStep On[]  = {
  {0,-1,&AllSelector, NULL, NULL, NULL, &lEaseOn}
};
StripStateStep Off[]  = {
  {0,-1,&AllSelector, NULL, NULL, NULL, &lEaseOff}
};

StripStateStep Test[]  = {
 // {0,-1,new BlockSelector(0,0,0,0), NULL, NULL, NULL, &lEaseOnHalf}
  {0,2000, &Col123, NULL, NULL, NULL, &lEaseOnHalf},
  {2000,4000, &Col123, NULL, NULL, NULL, &lEaseOffHalf},
  {500,2500,&Col456, &lEaseOnHalf, NULL, NULL, NULL},
  {2500,4500,&Col456, &lEaseOffHalf, NULL, NULL, NULL},
  {1000,3000,&Col78, NULL, &lEaseOnHalf, NULL, NULL},
  {3000,5000,&Col78, NULL, &lEaseOffHalf, NULL, NULL},
  {1500,3500,&Col910, NULL, NULL, &lEaseOnHalf, NULL},
  {3500,5500,&Col910, NULL, NULL, &lEaseOffHalf, NULL},
  {0,9000,&Row12, NULL, &lEaseOnHalf, &lEaseOnHalf, NULL},
  {1500,9000,&Row34, &lEaseOnHalf, NULL, &lEaseOnHalf, NULL},
  {3000,9000,&Row56, &lEaseOnHalf, &lEaseOnHalf, NULL, NULL},
};


StripState OnState = StripState(&strip, On, sizeof(On) / sizeof(On[0]));
StripState OffState = StripState(&strip, Off, sizeof(Off) / sizeof(On[0]));
StripState FastOnState = StripState(&strip, On, sizeof(On) / sizeof(On[0]));
StripState TestState = StripState(&strip, Test, sizeof(Test) / sizeof(On[0]));


void setup(){
  Serial.begin(9600);
  pinMode(BOT_1_PIN, OUTPUT);
  pinMode(BOT_2_PIN, OUTPUT);
  pinMode(BOT_3_PIN, OUTPUT);
  pinMode(BOT_4_PIN, OUTPUT);
  pinMode(TOP_1_PIN, OUTPUT);
  pinMode(TOP_2_PIN, OUTPUT);
  pinMode(TOP_3_PIN, OUTPUT);

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
      if (selectedState == -1) {
        selectThing(serial_char);
      } else {
        selectMode(serial_char);
      }
    }

    for (int i = 0; i < NUM_STATES; i++)
    {
        State* s = States[i];
        if (s != NULL)
        {
          s->update();
        }
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
  if (selectedState < NUM_PINS) {
    selectButtonMode(mode);
  } else if (selectedState == STRIP_STATE) {
    selectStripMode(mode);
  }

  selectedState = -1;
}

void selectThing(char thing) {
  switch (thing) {
    case '1':
      selectedState = 0;
      selectedPin = BOT_1_PIN;
      break;
    case '2':
      selectedState = 1;
      selectedPin = BOT_2_PIN;
      break;
    case '3':
      selectedState = 2;
      selectedPin = BOT_3_PIN;
      break;
    case '4':
      selectedState = 3;
      selectedPin = BOT_4_PIN;
      break;
    case 'a':
      selectedState = 4;
      selectedPin = TOP_1_PIN;
      break;
    case 'b':
      selectedState = 5;
      selectedPin = TOP_2_PIN;
      break;
    case 'c':
      selectedState = 6;
      selectedPin = TOP_3_PIN;
      break;
    case 's':
      selectedState = STRIP_STATE;
      break;
  }
}

void selectStripMode(char mode) {
  switch (mode) {
    case 'f':
      Serial.println("Entering Off State");
      States[selectedState] = &OffState;
      break;
    case 'm':
      Serial.println("Entering On State");
      States[selectedState] = &FastOnState;
      break;
    case 'n':
      Serial.println("Entering On State");
      States[selectedState] = &OnState;
      break;
    case 't':
      Serial.println("Entering Test State");
      States[selectedState] = &TestState;
      break;
    case 'u':

      break;
    case '1':

      break;
    case '2':

      break;
    case '3':

      break;
    case '4':

      break;
  }
  States[selectedState]->reset();

}

void selectButtonMode(char mode) {
    switch (mode) {
      case 'n':
      {
        States[selectedState] = new OnOffPinState(selectedPin, true);
      }
        break;
      case 'f':
      {
        States[selectedState] = new OnOffPinState(selectedPin, false);
      }
        break;
      case 'b':
      {
        States[selectedState] = new BlinkPinState(selectedPin, 500, 250);
      }
        break;
      case 's':
      {
        States[selectedState] = new BlinkPinState(selectedPin, 200, 100);
      }
        break;
    }
    selectedPin = -1;
}
//
// void displayUp(unsigned long currentTime, unsigned long timePassed)
// {
//   uint8_t minLight = (currentTime / UP_SPEED) % UP_REPEAT;
//   uint8_t maxLight = (currentTime / UP_SPEED + UP_LENGTH) % UP_REPEAT;
//
//   for(uint8_t i=0; i<strip.numPixels(); i++) {
//     uint8_t light = i % UP_REPEAT;
//     if ((minLight < maxLight && light > minLight && light <= maxLight) || (minLight > maxLight && (light <= maxLight || light > minLight)))
//     {
//       strip.setPixelColor(i, strip.Color(0,0,0,255));
//     }
//     else
//     {
//       strip.setPixelColor(i, strip.Color(0,0,0,0));
//     }
//   }
//   strip.show();
// }
//
// void displayOn(unsigned long currentTime, unsigned long timePassed)
// {
//   if ( timePassed < ON_OFF_DURATION) {
//     for(uint8_t i=0; i<strip.numPixels(); i++) {
//       int val = onOffEase.easeIn(timePassed) - i * ON_OFF_FADE;
//       strip.setPixelColor(i, strip.Color(0,0,0,constrain(val, 0, 255)));
//     }
//     strip.show();
//   }
// }
// void displayOff(unsigned long currentTime, unsigned long timePassed)
// {
//   if ( timePassed < ON_OFF_DURATION) {
//     for(uint8_t i=0; i<strip.numPixels(); i++) {
//       int val = 255 - constrain(onOffEase.easeOut(timePassed) - i * ON_OFF_FADE, 0, 255);
//       strip.setPixelColor(i, strip.Color(0,0,0,val));
//     }
//     strip.show();
//   }
// }
// void displayRed(unsigned long currentTime, unsigned long timePassed)
// {
//   timePassed = timePassed % (HEARTBEAT_DURATION * 5);
//
//   if (timePassed > 0 && timePassed < HEARTBEAT_DURATION)
//   {
//
//   }
//   else
//   {
//     timePassed -= HEARTBEAT_DURATION;
//   }
//
//   if (timePassed > 0 && timePassed < HEARTBEAT_DURATION)
//   {
//     for(uint8_t i=0; i<strip.numPixels(); i++) {
//       int val = heartEase.easeIn(timePassed);
//       strip.setPixelColor(i, strip.Color(0,constrain(val, 0, 255),0,0));
//     }
//   }
//   else
//   {
//     timePassed -= HEARTBEAT_DURATION;
//   }
//
//   if (timePassed > 0 && timePassed > HEARTBEAT_DURATION)
//   {
//     for(uint8_t i=0; i<strip.numPixels(); i++) {
//       int val = 255 - heartEase.easeOut(timePassed);
//       strip.setPixelColor(i, strip.Color(0, constrain(val, 0, 255),0,0));
//     }
//   }
//   else
//   {
//     timePassed -= HEARTBEAT_DURATION;
//   }
//
//   if (timePassed > 0 && timePassed < HEARTBEAT_DURATION)
//   {
//     for(uint8_t i=0; i<strip.numPixels(); i++) {
//       int val = heartEase.easeIn(timePassed);
//       strip.setPixelColor(i, strip.Color(0, constrain(val, 0, 255),0,0));
//     }
//   }
//   else
//   {
//     timePassed -= HEARTBEAT_DURATION;
//   }
//
//   if (timePassed > 0 && timePassed > HEARTBEAT_DURATION)
//   {
//     for(uint8_t i=0; i<strip.numPixels(); i++) {
//       int val = 255 - heartEase.easeOut(timePassed);
//       strip.setPixelColor(i, strip.Color(0, constrain(val, 0, 255),0,0));
//     }
//   }
//   else
//   {
//     timePassed -= HEARTBEAT_DURATION;
//   }
//
//   strip.show();
// }
//
// void displayYellow(unsigned long currentTime, unsigned long timePassed)
// {
//   if ( timePassed < SUN_DURATION * 1.5) {
//     for(uint8_t i=0; i<strip.numPixels(); i++) {
//       int val = sunEase.easeIn(timePassed) - (int(i / 10) * SUN_FADE);
//       strip.setPixelColor(i, strip.Color(constrain(val, 0, 255) * 0.5f,constrain(val, 0, 255),0,0));
//     }
//   }
//
//   if ( timePassed > SUN_DURATION * 1.5 && timePassed < MODE_DURATION) {
//     for(uint8_t i=0; i<strip.numPixels(); i++) {
//       int val = 255 - constrain(sunEase.easeOut(timePassed - SUN_DURATION * 1.5) - (int(((strip.numPixels() - 1) - i) / 10) * SUN_FADE), 0, 255);
//       strip.setPixelColor(i, strip.Color(val * 0.5f,val,0,0));
//     }
//   }
//   strip.show();
// }
//
// void displayGreen(unsigned long currentTime, unsigned long timePassed)
// {
//   uint8_t step = MODE_DURATION / strip.numPixels();
//
//   if ( timePassed < MODE_DURATION / 2)
//   {
//     for(uint8_t i=0; i<strip.numPixels()/2; i++) {
//       if (timePassed > i * step) {
//         strip.setPixelColor(i, strip.Color(255,0,0,0));
//         strip.setPixelColor((strip.numPixels() - 1) - i, strip.Color(255,0,0,0));
//       }
//     }
//   }
//   else
//   {
//     for(uint8_t i=0; i<strip.numPixels()/2; i++) {
//       int s = strip.numPixels()/2 - 1 - i;
//       if ((timePassed - MODE_DURATION / 2) > s * step) {
//         strip.setPixelColor(i, strip.Color(0,0,0,0));
//         strip.setPixelColor((strip.numPixels() - 1) - i, strip.Color(0,0,0,0));
//       }
//     }
//   }
//   strip.show();
// }
//
// void displayBlue(unsigned long currentTime, unsigned long timePassed)
// {
//   if (timePassed < MODE_DURATION - MEDIUM_FADE_DURATION)
//   {
//     //pick a random pixel every ... some amount of time.  turn it on
//     if (timePassed % 10 == 0)
//     {
//       int i = random(0, strip.numPixels());
//       strip.setPixelColor(i, strip.Color(0, 0, 255 ,0));
//     }
//
//     //cool stuff
//   }
//   else if (timePassed > MODE_DURATION - MEDIUM_FADE_DURATION && timePassed < MODE_DURATION)
//   {
//     for(uint8_t i=0; i<strip.numPixels(); i++) {
//       int val = 255 - constrain(mediumEase.easeOut(timePassed - (MODE_DURATION - MEDIUM_FADE_DURATION)), 0, 255);
//       strip.setPixelColor(i, strip.Color(0, 0, val ,0));
//     }
//   }
//   strip.show();
// }
