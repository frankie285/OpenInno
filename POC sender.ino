#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

int msg[1];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
void setup(void){
  Serial.begin(9600);
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(108);
  radio.openWritingPipe(pipe);
  radio.disableCRC();
  }
void loop(void){
  String theMessage = "I want to send a very large message because that is what a glowstick should do or not.";
  int messageSize = theMessage.length();
  for (int i = 0; i < messageSize; i++) {
    int charToSend[1];
    charToSend[0] = theMessage.charAt(i);
    radio.write(charToSend,1);
  }  
  
  msg[0] = 2; 
  radio.write(msg,1);

  radio.powerDown(); 
  radio.powerUp();
}
