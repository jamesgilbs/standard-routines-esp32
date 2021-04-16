#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h> //https://www.arduino.cc/en/Reference/WiFi versao 1.2.6
#endif

//SSID network
#define SSID "ssid"

//Password network
#define PASSWORD "password"

void setup() {
  Serial.begin(115200);
  setupWiFi();
}

//Client WiFi
WiFiClient wifiClient;

void setupWiFi() {
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conected");
}

void loop() {
}
