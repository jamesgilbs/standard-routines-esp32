/*  
 *  Basic example to show how to read a BMP image from SPIFFS
 *  and display using Adafruit GFX
 *  
 *  Tested with esp32 devboard and 160x128 ST7735 display
 *  
 */

// Required libraries
#include <SPIFFS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPIFFS_ImageReader.h>

// Display interface configuration
//#define DISPLAY_DC 5 //A0 Data command
//#define DISPLAY_CS 22 //CS
//#define DISPLAY_MOSI 23 //SDA
//#define DISPLAY_CLK 18 //SCK
//#define DISPLAY_RST 21 //Reset

#define DISPLAY_DC 2 //A0 Data command
#define DISPLAY_CS 5 //CS
#define DISPLAY_MOSI 17 //SDA
#define DISPLAY_CLK 16 //SCK
#define DISPLAY_RST 4 //Reset

Adafruit_ST7735 display = Adafruit_ST7735(DISPLAY_CS, DISPLAY_DC, DISPLAY_MOSI, DISPLAY_CLK, DISPLAY_RST);

// Display backlight enable pin
#define TFT_BACKLIGHT_PIN GPIO_NUM_32

// Image reader
SPIFFS_ImageReader reader;


void setup() {

  Serial.begin(115200);

  // initialize SPIFFS
  if(!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1);
  }

  // initialize display and turn on backlight
  display.initR(INITR_BLACKTAB);                     
  delay(500);
  pinMode(TFT_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(TFT_BACKLIGHT_PIN, HIGH);

  // draw image
  reader.drawBMP("/minion.bmp", display, 0, 0);
}

void loop() {

}
