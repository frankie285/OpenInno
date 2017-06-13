/*
In dit proof of concept worden de lattitude en longitude via de GPS sensor opgehaald. DE GPS Sensor EM506 heeft 6 draden van links naar rechts:
1: LED status
2: GND
3: TX
4: RX
5: 5V
6: GND
zowel pin 2 als 6 moet verbonden zijn met GND, pin 2 met 5V en tot slot de TX op arduino pin 5 en RX op arduino pin 3 (pin 3 hoeft niet aangesloten te worden, omdat dit een communicatie naar de gps toe is, die overbodig is)
*/

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 3, TXPin = 5; //alleen de TX pin is nodig, er wordt namelijk alleen data uitgelezen.
static const uint32_t GPSBaud = 4800;

// The TinyGPS++ object
TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);

double LAT;
double LON;

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
}

void loop()
{
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    LAT = gps.location.lat();
    LON = gps.location.lng();
    Serial.print(LAT, 6);
    Serial.print(" , ");
    Serial.print(LON, 6);
    Serial.println();
  }
}