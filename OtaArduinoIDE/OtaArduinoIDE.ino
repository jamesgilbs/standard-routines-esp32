#ifdef ESP8266

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#else
#include <WiFi.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#endif

#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char*   SSID      = "ssid";
const char*   PASSWORD  = "password";

void setup() {
#ifdef ESP8266
  Serial.begin(74880);
#else
  Serial.begin(115200);
#endif
  Serial.println("\nGetting Started.");

  if (SPIFFS.begin()) {
    Serial.println("SPIFFS Ok");
  } else {
    Serial.println("SPIFFS fail");
  }

  if (SPIFFS.exists("/Test.txt")) {
    File f = SPIFFS.open("/Test.txt", "r");
    if (f) {
      Serial.println("Read file:");
      Serial.println(f.readString());
      f.close();
    }
  } else {
    Serial.println("File not found.");
  }

  WiFi.begin(SSID, PASSWORD);
  Serial.println("\nConect WiFi " + String(SSID));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  /*Allows you to define port for connection
      Standard: ESP8266 - 8266
                ESP32   - 3232              */
  //ArduinoOTA.setPort(port);

  /*Allows you to define host name
      Standard: ESP8266 - esp8266-[ChipID]
                ESP32   - esp32-[MAC]       */
  //ArduinoOTA.setHostname("host");

  /*Allows you to set a password for access
      Standard: not password                */
  //ArduinoOTA.setPassword("password");

  /*Allows to set password for access via Hash MD5
      Standard: not password               */
  //ArduinoOTA.setPasswordHash("passwordHashMD5");

  //Defines process callback functions
  ArduinoOTA.onStart([]() {
    String s;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      //Update sketch
      s = "Sketch";
    } else {
      //Update SPIFFS
      s = "SPIFFS";
      //SPIFFS must be completed
      SPIFFS.end();
    }
    Serial.println("Getting Started OTA - " + s);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA Concluded.");
  });

  //Progress
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int overall) {
    Serial.print(progress * 100 / overall);
    Serial.print(" ");
  });

  //Fail
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.print("Erro " + String(error) + " ");
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Authorization failure");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Initialization failure");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connection failure");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receiving failure");
    } else if (error == OTA_END_ERROR) {
      Serial.println("Finalization failure");
    } else {
      Serial.println("Unknown failure");
    }
  });

  //Initializes OTA
  ArduinoOTA.begin();

  //Ready
  Serial.println("Update via OTA available.");
  Serial.print("IP Adress: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
}
