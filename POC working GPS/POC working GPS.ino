/*
In dit proof of concept worden de lattitude en longitude via de GPS sensor opgehaald. DE GPS Sensor EM506 heeft 6 draden van links naar rechts:
1: LED status
2: GND
3: TX
4: RX
5: 5V
6: GND
zowel pin 2 als 6 moet verbonden zijn met GND, pin 2 met 5V en tot slot de TX op arduino pin 5 en RX op arduino pin 3
*/

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

int RXPin = 3;
int TXPin = 5;
SoftwareSerial gpsSerial(RXPin, TXPin);

int GPSBaud = 4800;

double lattitude;
double longitude;

TinyGPSPlus gps;

void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
}

void loop()
{
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    lattitude = gps.location.lat();
    longitude = gps.location.lng();
    Serial.println(lattitude + ':' + longitude);
  }
}
