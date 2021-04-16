
#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu6050(Wire);

#define I2C_SDA 14
#define I2C_SCL 15

#define pinLedXR 32
#define pinLedXL 33

float anguloX;
float anguloY;
float anguloZ;

void setup() {
  Serial.begin(115200);
  
  pinMode(pinLedXR, OUTPUT);
  pinMode(pinLedXL, OUTPUT);
  
  Wire.begin(I2C_SDA, I2C_SCL); //Pinos de I2C
  
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  
  digitalWrite(pinLedXR, HIGH); // Leds piscan para mostrar inicio
  digitalWrite(pinLedXL, HIGH);
  delay(500);
  digitalWrite(pinLedXR, LOW);
  digitalWrite(pinLedXL, LOW);
}

void loop() {
  mpu6050.update();
  anguloX = mpu6050.getAngleX();
  anguloY = mpu6050.getAngleY();
  anguloZ = mpu6050.getAngleZ();

  if (anguloX >= 10) {
    Serial.println("Rampa detectada, subindo...");
    digitalWrite(pinLedXR, HIGH);
  } else if (anguloX <= -10) {
    Serial.println("Rampa detectada, descendo...");
    digitalWrite(pinLedXL, HIGH);
  } else {
    digitalWrite(pinLedXR, LOW);
    digitalWrite(pinLedXL, LOW);
  
  Serial.print("angleX :");
  Serial.print(anguloX);
  Serial.print("\tangleY :");
  Serial.print(anguloY);
  Serial.print("\tangleZ :");
  Serial.println(anguloZ);

  }
}
