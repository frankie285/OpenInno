#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

int msg[1];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
void setup(void){
  Serial.begin(38400);
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(108);
  radio.openWritingPipe(pipe);
  radio.disableCRC();
  }
void loop(void){

  if(Serial.available() > 0){
    msg[0] = Serial.read();
    radio.write(msg, 1);
  }
  
  //msg[0] = 255; 
  //radio.write(msg,1);

  radio.powerDown(); 
  radio.powerUp();
}
