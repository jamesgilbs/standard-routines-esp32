// Color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

void setCursor(uint16_t x0, uint16_t y0);
void setTextColor(uint16_t color);
void setTextColor(uint16_t color, uint16_t backgroundcolor);
void setTextSize(uint8_t size);
void setTextWrap(boolean w);
void fillScreen(uint16_t color); //Erase Full
void setRotation(uint8_t rotation);

uint16_t width(); //Reference Display Size
uint16_t height();


void drawPixel(uint16_t x, uint16_t y, uint16_t color); //Pixel
void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color); //Line
void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color); //Void Rectangle
void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color); //Fill Rectangle
void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color); //Void Circle
void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color); //Fill Circle
void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color); //Void Round Rectangle
void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color); //Fill Round Rectangle
void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color); //Void Circle
void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color); //Fill Circle

//Draw Char
void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size); //Char

//Char (Direct Text)
void setCursor ( uint16_t x0 , uint16_t y0 ); 
void setTextColor ( uint16_t color );
void setTextColor ( uint16_t color , uint16_t backgroundcolor ); 
void setTextSize ( uint8_t size );
void setTextWrap ( boolean w );

//Bitmap
void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

//Fonts
#include <Fonts/FreeMonoBoldOblique12pt7b.h> //Libraries Font
tft.setFont(&FreeMonoBoldOblique12pt7b); //Set Font
tft.setFont(); //Return Default Font

FreeMono12pt7b.h FreeSansBoldOblique12pt7b.h
FreeMono18pt7b.h FreeSansBoldOblique18pt7b.h
FreeMono24pt7b.h FreeSansBoldOblique24pt7b.h
FreeMono9pt7b.h FreeSansBoldOblique9pt7b.h
FreeMonoBold12pt7b.h FreeSansOblique12pt7b.h
FreeMonoBold18pt7b.h FreeSansOblique18pt7b.h
FreeMonoBold24pt7b.h FreeSansOblique24pt7b.h
FreeMonoBold9pt7b.h FreeSansOblique9pt7b.h
FreeMonoBoldOblique12pt7b.h FreeSerif12pt7b.h
FreeMonoBoldOblique18pt7b.h FreeSerif18pt7b.h
FreeMonoBoldOblique24pt7b.h FreeSerif24pt7b.h
FreeMonoBoldOblique9pt7b.h FreeSerif9pt7b.h
FreeMonoOblique12pt7b.h FreeSerifBold12pt7b.h
FreeMonoOblique18pt7b.h FreeSerifBold18pt7b.h
FreeMonoOblique24pt7b.h FreeSerifBold24pt7b.h
FreeMonoOblique9pt7b.h FreeSerifBold9pt7b.h
FreeSans12pt7b.h FreeSerifBoldItalic12pt7b.h
FreeSans18pt7b.h FreeSerifBoldItalic18pt7b.h
FreeSans24pt7b.h FreeSerifBoldItalic24pt7b.h
FreeSans9pt7b.h FreeSerifBoldItalic9pt7b.h
FreeSansBold12pt7b.h FreeSerifItalic12pt7b.h
FreeSansBold18pt7b.h FreeSerifItalic18pt7b.h
FreeSansBold24pt7b.h FreeSerifItalic24pt7b.h
FreeSansBold9pt7b.h FreeSerifItalic9pt7b.h
