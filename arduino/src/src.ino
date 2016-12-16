#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIXEL_PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);
char serial_char = 0;

void setup(){
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  
  strip.begin();
  strip.show();
}

void loop(){
  if (Serial.available()) {
    serial_char = Serial.read();
    if (serial_char == 'w')
    {
      all_white(255);
    }
    else if (serial_char == 'd')
    {
      all_white(100);
    }
  }
  
}

void all_white(uint8_t b) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,0,0,b));
    strip.show();
  }
}


