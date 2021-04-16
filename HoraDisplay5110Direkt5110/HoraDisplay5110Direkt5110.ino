    /////////////////////////////////////////////////////////////////
   //              Arduino E-Paper Display Tutorial     v1.02     //
  //       Get the latest version of the code here:              //
 //        http://educ8s.tv/arduino-realtime-ds1302             //
/////////////////////////////////////////////////////////////////

#include <LCD5110_Graph.h>
#include <stdio.h>
#include <DS3231.h>
#include "LowPower.h"

LCD5110 lcd(8,9,10,12,11);
extern unsigned char BigNumbers[];
extern unsigned char SmallFont[];
extern unsigned char TinyFont[];
extern unsigned char MediumNumbers[];
extern uint8_t clock[];
DS3231  rtc(SDA, SCL);



void setup() {
   rtc.begin();
 //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(12, 0, 0); 
  // rtc.setDate(9, 10, 2019);
pinMode(12,OUTPUT);
digitalWrite(12,0);
pinMode(11,OUTPUT);
digitalWrite(12,0);
pinMode(10,OUTPUT);
digitalWrite(12,1);
 lcd.InitLCD();
 lcd.clrScr();
 lcd.setContrast(99);
// lcd.drawBitmap(0, 0, clock, 84, 48);
  
 lcd.update();  
 
}
String h;
String m;
String secc;
String d;
String broj;
String volts="5";
Time  t;

void loop() {
lcd.clrScr();
 t = rtc.getTime();

h=String(t.hour);
if(t.hour<10)
h="0"+h;

m=String(t.min);
if(t.min<10)
m="0"+m;

secc=String(t.sec);
if(t.sec<10)
secc="0"+secc;
 //lcd.drawLine(0,46,duljina,46);
 //lcd.drawRect(50,18,83,40);
 
  lcd.setFont(SmallFont);
  lcd.print(rtc.getDateStr(),10,4);

   lcd.setFont(TinyFont);
   //lcd.print(rtc.getDOWStr(),0,10);

  
 lcd.setFont(BigNumbers);
  lcd.print(h,0,20);
  lcd.print(m,32,20);
   lcd.drawRect(29,28,30,29);
   lcd.drawRect(29,34,30,35);

  
    lcd.setFont(MediumNumbers);
     lcd.print(secc,60,20);
lcd.drawLine(0,46,84,46);
lcd.drawLine(0,47,84,47);
lcd.drawLine(0,16,84,16);
lcd.drawLine(0,15,84,15);

lcd.drawLine(0,0,84,0);

  
 

  
    lcd.update();
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); 
}
