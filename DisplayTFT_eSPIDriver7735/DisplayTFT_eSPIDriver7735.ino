#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI.git 2.2.14
#include <SPI.h>

//Pinning on User_Setup.h in the library folder
TFT_eSPI display = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  setupDisplay(); //Initializes and configures the display
  delay(2000);
  display.fillScreen(TFT_BLUE);
  display.setCursor(0, 0);
}

void setupDisplay() {
  display.init();
  display.setRotation(1);
  display.fillScreen(TFT_BLUE); //Clean the display with the color blue
  display.setTextColor(TFT_WHITE, TFT_BLUE); //Puts text as white with blue background
  display.setTextWrap(true, true);//Enable line break
  display.setTextSize(1);
  display.setCursor(0, 0, 2); //X, y position and text font
  display.println("Display Ok");
}

void loop() {
  display.setCursor(0, 0, 2);
  display.println("Test display");
}
