/*
In dit proof op concept wordt er een ledstrip aangestuurd vanuit een Arduino. De ws2812b ledstrip bevat 3 draden: +, -, data. 
Om dit proof of concept te laten werken, wordt de + (rood) op de 5V aangesloten, de - (zwart) wordt op GND aangesloten en tot slot wordt de data (groen) op digital pin 5 aangesloten.
De volgende code zorgt ervoor dat er 5 leds aan gaan en de kleur wit krijgen. vervolgens kan met de functie setColor de RGB waardes van de leds aangepast worden.
*/

#include "FastLED.h"

#define NUM_LEDS 5 //aantal leds aan te sturen
#define DATA_PIN 6 //pin waarop de data kabel van de strip wordt aangesloten.
CRGB leds[5]; // ledstrip object.


void setup() { 
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  for(int i = 0; i < NUM_LEDS; i++){ // de 5 leds worden aangezet op wit.
     leds[i] = CRGB(255,255,255); 
  }
    FastLED.show();
}

void loop() { 
  setColor(100,0,1); //in deze functie worden de RGB waardes ingevuld. Dit laat de ledstrip van kleur veranderen.
}


void setColor(int r, int g, int b){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(r,g,b); 
  }
  FastLED.show();
}


