#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "FastLED.h"

#define NUM_LEDS 5 //aantal leds aan te sturen
#define DATA_PIN 6 //pin waarop de data kabel van de strip wordt aangesloten.
CRGB leds[5]; // ledstrip object.

#define DEBUG 1  //Set to 0 for no debugging text

int msg[1];
RF24 radio(9, 10);
static const int RXPin = 3, TXPin = 5;
static const uint32_t GPSBaud = 4800;
const uint64_t pipe = 0xE8E8F0F0E1LL;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

double GPSLat;
double GPSLon;

int myX;
int myY;

byte startByte = 100;
byte stopByte = 200;
byte header[22];
byte pixel[7];
byte* pixelArrayPtr = 0;
int headerIndex = 0;
int pixelIndex = 0;
int pixelCounter = 0;

enum states {
  START = 1,
  DATA = 2,
  CHECKHEADER = 3,
  PIXEL = 4
};

typedef struct {
  double nwLat;
  double nwLon;
  double seLat;
  double seLon;
  int pixVert;
  int pixHor;
} structHeader;

structHeader cHeader;

typedef struct {
  int xPixel;
  int yPixel;
  int red;
  int green;
  int blue;
} structPixel;

structPixel cPixel;

states currentState = START;


void setup(void) {
  Serial.begin(38400);
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(108);
  radio.openReadingPipe(1, pipe);
  radio.disableCRC();
  radio.startListening();

  ss.begin(GPSBaud);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  for (int i = 0; i < NUM_LEDS; i++) { // de 5 leds worden aangezet op wit.
    leds[i] = CRGB(255, 255, 255);
  }
  FastLED.show();
}
void loop(void) {
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (radio.available()) {
    radio.read(msg, 1);
    byte readByte = msg[0];

    switch (currentState) {
      case START:

        headerIndex = 0;

        if (readByte == startByte) {
          Serial.println("header found");
          header[headerIndex] = readByte;
          headerIndex++;
          currentState = DATA;
        }
        break;

      case DATA:
        header[headerIndex] = readByte;
        headerIndex++;

        if (headerIndex == 22) //Header vol
        {
          if (header[21] == stopByte) //Check stopbyte
          {
            fillHeader();
            if (checkLocation()) {
              myXAndY(&myX, &myY);
              currentState = PIXEL; //Header is goed -> haal pixel data op
              break;
            }
          }
          else
          {
            currentState = START;
          }
        }
        break;

      case PIXEL:
        pixel[pixelIndex] = readByte;
        pixelIndex++;
        if (pixelIndex >= 7) {
          pixelIndex = 0;
          pixelCounter++;

          cPixel.xPixel = arrayToInt(pixel[0], pixel[1]);
          cPixel.yPixel = arrayToInt(pixel[2], pixel[3]);
          cPixel.red = pixel[4];
          cPixel.green = pixel[5];
          cPixel.blue = pixel[6];

          /*
            Serial.print("xWaarde: ");
            Serial.print(cPixel.xPixel);
            Serial.print(" yWaarde: ");
            Serial.print(cPixel.yPixel);
            Serial.print(" red: ");
            Serial.print(cPixel.red);
            Serial.print(" green:");
            Serial.print(cPixel.green);
            Serial.print(" blue: ");
            Serial.print(cPixel.blue);
            Serial.println();
          */


          if (cPixel.xPixel == myX && cPixel.yPixel == myY) {
            setColor(cPixel.red, cPixel.green, cPixel.blue);
            Serial.print(cPixel.red);
            Serial.print(",");
            Serial.print(cPixel.green);
            Serial.print(",");
            Serial.print(cPixel.blue);
            Serial.println();

            currentState = START;
          }


          if (pixelCounter >= (cHeader.pixVert * cHeader.pixHor)) {
            pixelCounter = 0;
            currentState = START;
          }
        }
        break;
    }
  }
}

double arrayToLong(byte a, byte b, byte c, byte d)
{
  long value = 0;

  value |= (long) a << 24;
  value |=  (long) b << 16;
  value |= (long) c << 8;
  value |= (long) d;
  return (double)value / 100000;
}

int arrayToInt(byte a, byte b)
{
  int value = 0;
  value |= (int) a << 8;
  value |= (int) b;
  return value;
}

void fillHeader() {
  cHeader.nwLat = arrayToLong(header[1], header[2], header[3], header[4]);
  cHeader.nwLon = arrayToLong(header[5], header[6], header[7], header[8]);
  cHeader.seLat = arrayToLong(header[9], header[10], header[11], header[12]);
  cHeader.seLon = arrayToLong(header[13], header[14], header[15], header[16]);
  cHeader.pixVert = arrayToInt(header[17], header[18]);
  cHeader.pixHor = arrayToInt(header[19], header[20]);
  /*
    if (DEBUG) {
    Serial.print("NWLAT: ");
    Serial.println(cHeader.nwLat, 5);
    Serial.print("NWLON: ");
    Serial.println(cHeader.nwLon, 5);
    Serial.print("SELAT: ");
    Serial.println(cHeader.seLat, 5);
    Serial.print("SELON: ");
    Serial.println(cHeader.seLon, 5);
    Serial.print("PIXVERT: ");
    Serial.println(cHeader.pixVert);
    Serial.print("PIXHON: ");
    Serial.println(cHeader.pixHor);
    }
  */
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    GPSLat = gps.location.lat();
    GPSLon = gps.location.lng();
  }
}

bool checkLocation() {
  if (GPSLat <= cHeader.nwLat && GPSLat >= cHeader.seLat && GPSLon >= cHeader.nwLon && GPSLon <= cHeader.seLon) {
    return true;
  }
  else {
    return false;
  }
}

void myXAndY(int* x, int* y) {
  double pixelHeight = (cHeader.nwLat - cHeader.seLat) / cHeader.pixVert;
  double pixelWidth = (cHeader.seLon - cHeader.nwLon) / cHeader.pixHor;

  double distanceToNwLat = cHeader.nwLat - GPSLat;
  double distanceToNwLon = GPSLon - cHeader.nwLon;

  *y = (int)(distanceToNwLat / pixelHeight);
  *x = (int)(distanceToNwLon / pixelWidth);
}

void setColor(int r, int g, int b){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(r,g,b); 
  }
  FastLED.show();
}
