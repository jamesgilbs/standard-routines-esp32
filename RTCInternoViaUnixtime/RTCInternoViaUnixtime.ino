#include <Arduino.h>
#include <sys/time.h>

int setUnixtime(int32_t unixtime) {
  timeval epoch = {unixtime, 0};
  return settimeofday((const timeval*)&epoch, 0);
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println(&timeinfo,"%H:%M:%S");
  Serial.println(&timeinfo,"%H:%M");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Test Set and Read RTC"); 
 // Unix time = seconds since Jan 01 1970. (UTC)
  // example: 1583390646 <= (2020-03-05 06:44:06Z)
  setUnixtime(1603572780); 
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  printLocalTime();
}
