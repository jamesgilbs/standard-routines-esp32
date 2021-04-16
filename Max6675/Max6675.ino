#include <Arduino.h>
#include <max6675.h>

int thermoDO = 19; //SO
int thermoCS = 23; //CS
int thermoCLK = 5; //SCK

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {
  Serial.begin(9600);
  Serial.println("MAX6675 init.");
}

void loop() {

  Serial.print("Hot: ");
  Serial.println(thermocouple.readCelsius() * 0.90); //thermocouple.readFahrenheit()
  Serial.println("");

  delay(1000);
}
