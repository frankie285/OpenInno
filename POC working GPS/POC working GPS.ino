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
  }
}
