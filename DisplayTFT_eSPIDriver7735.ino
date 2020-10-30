//Change file TFT_eSPI \ User_Setup.h

/*1° - #define ST7735_DRIVER      // Define additional parameters below for this display

  2° - // For ESP32 Dev board (only tested with ILI9341 display)
  // The hardware SPI can be mapped to any pins

  #define TFT_MISO 19
  #define TFT_MOSI 23
  #define TFT_SCLK 18
  #define TFT_CS   22  // Chip select control pin
  #define TFT_DC   5  // Data Command control pin
  #define TFT_RST   21  // Reset pin (could connect to RST pin)
  //#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

  //#define TOUCH_CS 21     // Chip select pin (T_CS) of touch screen

  //#define TFT_WR 22    // Write strobe for modified Raspberry Pi TFT only
*/

#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI.git 2.2.14
/*
  User_Setup.h
  #define TFT_MISO 19
  #define TFT_MOSI 23
  #define TFT_SCLK 18
  #define TFT_CS   22  // Chip select control pin
  #define TFT_DC   5  // Data Command control pin
  #define TFT_RST   21  // Reset pin (could connect to RST pin)
*/

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
