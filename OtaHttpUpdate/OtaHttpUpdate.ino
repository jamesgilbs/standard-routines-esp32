#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <FS.h>
#else
#include <WiFi.h>
#include <HTTPUpdate.h>
#include <SPIFFS.h>
#endif

const char*   SSID      = "ssid";
const char*   PASSWORD  = "password";

void setup() {
#ifdef ESP8266
  Serial.begin(74880);
#else
  Serial.begin(115200);
#endif
  Serial.println("\nStarting");

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
    Serial.println("File not found");
  }

  WiFi.begin(SSID, PASSWORD);
  Serial.println("\nConect WiFi " + String(SSID));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

#ifdef ESP8266
  //LED indicator prograss
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

  //Callback begin
  ESPhttpUpdate.onStart([] {
    Serial.println("Update started (callback)");
  });

  //Callback end
  ESPhttpUpdate.onEnd([] {
    Serial.println("\nUpdate complete (callback)");
  });

  //Callback error
  ESPhttpUpdate.onError([](int error) {
    Serial.println("\nUpdate error (callback), Error: " + String(error));
  });

  //Callback progress
  ESPhttpUpdate.onProgress([](size_t progress, size_t total) {
    Serial.print(progress * 100 / total);
    Serial.print(" ");
  });

  //Do not restart automatically
  ESPhttpUpdate.rebootOnUpdate(false);
#else
  //ESP32 definitions
  //LED indicator prograss
  httpUpdate.setLedPin(2, HIGH);

  //Callback progress
  Update.onProgress([](size_t progress, size_t total) {
    Serial.print(progress * 100 / total);
    Serial.print(" ");
  });

  //Do not restart automatically
  httpUpdate.rebootOnUpdate(false);
#endif

  //Create secure Client instance
  WiFiClientSecure client;
#ifdef ESP8266
  // ESP8266
  client.setInsecure();
#endif

  //Update SPIFFS
  Serial.println("\n*** Update SPIFFS ***");
  for (byte b = 5; b > 0; b--) {
    Serial.print(b);
    Serial.println("... ");
    delay(1000);
  }
  //Close SPIFFS
  SPIFFS.end();

  //Update SPIFFS
#ifdef ESP8266
  // ESP8266
  t_httpUpdate_return result = ESPhttpUpdate.updateSpiffs(client, "Cloud path *.bin"); //Path in the cloud
#else
  // ESP32
  t_httpUpdate_return result = httpUpdate.updateSpiffs(client, "Cloud path *.bin"); //Path in the cloud
#endif

  //Check result
  switch (result) {
    case HTTP_UPDATE_FAILED: {
#ifdef ESP8266
        // ESP8266
        String s = ESPhttpUpdate.getLastErrorString();
#else
        // ESP32
        String s = httpUpdate.getLastErrorString();
#endif
        Serial.println("\nFail: " + s);
      } break;
    case HTTP_UPDATE_NO_UPDATES: {
        Serial.println("\nNo updates available");
      } break;
    case HTTP_UPDATE_OK: {
        Serial.println("\nSucess");
      } break;
  }

  //Initializes SPIFFS
  if (SPIFFS.begin()) {
    Serial.println("SPIFFS Ok");
  } else {
    Serial.println("SPIFFS fail");
  }

  //Checks/displays file
  if (SPIFFS.exists("/Test.txt")) {
    File f = SPIFFS.open("/Test.txt", "r");
    if (f) {
      Serial.println("Read fail:");
      Serial.println(f.readString());
      f.close();
    }
  } else {
    Serial.println("File not found");
  }

  //Update Sketch
  Serial.println("\n*** Update sketch ***");
  for (byte b = 5; b > 0; b--) {
    Serial.print(b);
    Serial.println("... ");
    delay(1000);
  }

  //Update Sketch
#ifdef ESP8266
  // ESP8266
  result = ESPhttpUpdate.update(client, "Cloud path *.bin"); //Path in the cloud
#else
  // ESP32
  result = httpUpdate.update(client, "Cloud path *.bin"); //Path in the cloud
#endif

  //Check result
  switch (result) {
    case HTTP_UPDATE_FAILED: {
#ifdef ESP8266
        // ESP8266
        String s = ESPhttpUpdate.getLastErrorString();
#else
        // ESP32
        String s = httpUpdate.getLastErrorString();
#endif
        Serial.println("\nFail: " + s);
      } break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("\nNo updates available");
      break;
    case HTTP_UPDATE_OK: {
        Serial.println("\nSucess, restarting ...");
        ESP.restart();
      } break;
  }
}

void loop() {
}
