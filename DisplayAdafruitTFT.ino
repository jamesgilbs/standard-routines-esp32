#include <Adafruit_GFX.h> //https://github.com/adafruit/Adafruit-GFX-Library.git versao 1.10.0
#include <Adafruit_ST7735.h> //https://github.com/adafruit/Adafruit-ST7735-Library.git versao 1.6.0

//Display pins
#define DISPLAY_DC 5 //A0 Data command
#define DISPLAY_CS 22 //CS
#define DISPLAY_MOSI 23 //SDA
#define DISPLAY_CLK 18 //SCK
#define DISPLAY_RST 21 //Reset

Adafruit_ST7735 display = Adafruit_ST7735(DISPLAY_CS, DISPLAY_DC, DISPLAY_MOSI, DISPLAY_CLK, DISPLAY_RST);

void setup() {
  setupDisplay();
}

void loop() {
  display.setCursor(0, 0);
}

void setupDisplay() {
  display.initR(INITR_BLACKTAB);                     //Initializes the display
  display.setRotation(1);                            //Rotate
  display.setTextSize(2);                            //Text size
  display.fillScreen(ST77XX_BLACK);                  //Fills with black color
  display.setCursor(0, 0);                           //Place the cursor at the beginning
  display.setTextColor(ST77XX_WHITE, ST77XX_BLACK);  //White text with black background
  display.setTextWrap(false);                        //Don't skip the line automatically
}
