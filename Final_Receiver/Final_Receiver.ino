#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define DEBUG 1  //Set to 0 for no debugging text

int msg[1];
RF24 radio(9,10);
static const int RXPin = 3, TXPin = 5; 
static const uint32_t GPSBaud = 4800;
const uint64_t pipe = 0xE8E8F0F0E1LL;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

double GPSLat;
double GPSLon;

byte startByte = 100;
byte stopByte = 200;
byte header[22];
byte* pixelArrayPtr = 0;
int headerIndex = 0;

enum states{
  START = 1,
  DATA = 2,
  CHECKHEADER = 3,
  PIXEL = 4
  };

typedef struct{
  double nwLat;
  double nwLon;
  double seLat;
  double seLon;
  int pixVert;
  int pixHor;
} structHeader;

structHeader cHeader;

states currentState = START;


void setup(void){
  Serial.begin(38400);
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(108);
  radio.openReadingPipe(1,pipe);
  radio.disableCRC();
  radio.startListening();
}
void loop(void){
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();
  
  if (radio.available()){
      radio.read(msg, 1);
      byte readByte = msg[0];

      switch(currentState){
        case START:

          headerIndex = 0;
        
          if(readByte == startByte){
            header[headerIndex] = readByte;
            headerIndex++;
            currentState = DATA;
            if(DEBUG)
            {
              Serial.println("Header: StartByte Found");
            }
          }
        break;

        case DATA:
          if(headerIndex < 22){
            header[headerIndex] = readByte;
            headerIndex++;  
          }
          else{
            currentState = CHECKHEADER;
          }
        break;

        case CHECKHEADER:
        if(header[21] == stopByte){
          fillHeader();
          currentState = PIXEL;
        }
        else{
          currentState = START;
        }

        break;

        case PIXEL:
          free(pixelArrayPtr);
          pixelArrayPtr = new byte[cHeader.pixVert * cHeader.pixHor * 7];
          

        currentState = START;
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

void fillHeader(){
  cHeader.nwLat = arrayToLong(header[1], header[2], header[3], header[4]);
          cHeader.nwLon = arrayToLong(header[5], header[6], header[7], header[8]);
          cHeader.seLat = arrayToLong(header[9], header[10], header[11], header[12]);
          cHeader.seLon = arrayToLong(header[13], header[14], header[15], header[16]);
          cHeader.pixVert = arrayToInt(header[17], header[18]);
          cHeader.pixHor = arrayToInt(header[19], header[20]);
          if(DEBUG){
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
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    GPSLat = gps.location.lat();
    GPSLon = gps.location.lng();
    if(DEBUG){
      Serial.print(GPSLat, 6);
      Serial.print(" , ");
      Serial.print(GPSLon, 6);
      Serial.println(); 
    }
  }
}

