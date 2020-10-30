"Send sms"
#include <Sim800l.h>
#include <SoftwareSerial.h>
Sim800l Sim800l;  //declare the library
char* text;
char* number;
bool error;


void setup(){
	Sim800l.begin();
	text="Testing Sms";
	number="+542926556644";
	error=Sim800l.sendSms(number,text);
	// OR
	//error=Sim800l.sendSms("+540111111111","the text go here");
}

void loop(){
	//do nothing
}
"Receiver sms"
#include <Sim800l.h>
#include <SoftwareSerial.h>
Sim800l Sim800l;
String text;
uint8_t index;


void setup(){
	Serial.begin(9600);          // only for debug the results .
	Sim800l.begin();             // initializate the library.
	index=1;                     // first position in the prefered memory storage.
	text=Sim800l.readSms(index);
	Serial.println(text);

}

void loop(){
	//do nothing
}




