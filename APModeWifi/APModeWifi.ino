#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h> //https://www.arduino.cc/en/Reference/WiFi versao 1.2.6
#endif

#include <WiFiManager.h> https://github.com/zhouhan0126/WIFIMANAGER-ESP32.git

//Flag to indicate whether a new network configuration has been saved
bool shouldSaveConfig = false;

const int PIN_AP = 26;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_AP, INPUT);

  WiFiManager wifiManager;

  //using this command, the settings are cleared from memory
  //if you have saved a network to connect automatically, it is deleted.
  //wifiManager.resetSettings();

  //by default the Debug messages will appear on the serial monitor, if you want to disable it
  //wifiManager.setDebugOutput(false);

  // if you want to start the Portal to connect to a network every time, without trying to connect
  // to a previously saved network, use startConfigPortal instead of autoConnect
  //wifiManager.startConfigPortal(char const *apName, char const *apPassword = NULL);

  //set fixed IP for ESP (must be set before autoConnect)
  //setAPStaticIPConfig(ip, gateway, subnet);
  //wifiManager.setAPStaticIPConfig(IPAddress(192,168,16,2), IPAddress(192,168,16,1), IPAddress(255,255,255,0)); //Mode AP

  //setSTAStaticIPConfig(ip, gateway, subnet);
  //wifiManager.setSTAStaticIPConfig(IPAddress(192,168,0,99), IPAddress(192,168,0,1), IPAddress(255,255,255,0)); //Mode Station

  //callback when entering AP configuration mode
  wifiManager.setAPCallback(configModeCallback);
  //callback when connecting to a network, that is, when working in station mode
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.autoConnect("ESP_AP", "12345678"); //Creates a passwordless network
  //wifiManager.autoConnect(); //Automatically generates the SSID with the ESP ID chip and without a password

  //wifiManager.setMinimumSignalQuality(10); //% Minimum for him to show on SCAN

  //wifiManager.setRemoveDuplicateAPs(false); //Remove duplicate networks (same SSID)

  //wifiManager.setConfigPortalTimeout(10); //Timeout for ESP not to wait to be configured forever

}

void loop() {
  WiFiManager wifiManager;
  //if the button was pressed
  if ( digitalRead(PIN_AP) == HIGH) {
    Serial.println("Reset"); //Try to open the portal
    if (!wifiManager.startConfigPortal("ESP_AP", "12345678") ) {
      Serial.println("Conect fail");
      delay(2000);
      ESP.restart();
      delay(1000);
    }
    Serial.println("Conected ESP_AP!!!");
  }
}

//callback that indicates that ESP has entered AP mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered configuration mode");
  Serial.println(WiFi.softAPIP()); //Print IP of AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //Print SSID
}

//callback indicating that we saved a new network to connect to (station mode)
void saveConfigCallback () {
  Serial.println("Configuration saved");
  Serial.println(WiFi.softAPIP());
}
