#include <SoftwareSerial.h> //https://github.com/plerup/espsoftwareserial.git versao 6.9.0

//Example using SIM800L over serial 1 and commanding over native serial 0
#define TX_PIN 4
#define RX_PIN 2

SoftwareSerial softSerial(TX_PIN, RX_PIN);

void setup(){
  Serial.begin(115200);

  softSerial.begin(9600);
  delay(1000);
}

void loop(){
  //Send new serial
  if (Serial.available()){
      softSerial.write(Serial.read());
  }

  //Received serial
  if (softSerial.available()){
      Serial.write(softSerial.read());
  }
}
