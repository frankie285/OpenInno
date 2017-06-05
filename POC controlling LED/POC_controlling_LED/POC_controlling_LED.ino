#include "FastLED.h"

#define NUM_LEDS 5
#define DATA_PIN 6
CRGB leds[5];


void setup() { 
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  for(int i = 0; i < NUM_LEDS; i++){
     leds[i] = CRGB(255,255,255); 
  }
    FastLED.show();
}

void loop() { 
  setColor(100,0,1);
}


void setColor(int r, int g, int b){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(r,g,b); 
  }
  FastLED.show();
}


