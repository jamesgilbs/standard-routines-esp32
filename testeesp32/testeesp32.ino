#include <WiFi.h>

const int ledPin = 2;
bool wifiDone = false;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n==== TESTE ESTAVEL ESP32 ====\n");

  pinMode(ledPin, OUTPUT);

  Serial.print("CPU: ");
  Serial.print(getCpuFrequencyMhz());
  Serial.println(" MHz");

  Serial.print("Heap: ");
  Serial.println(ESP.getFreeHeap());

  // WiFi init
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(500);

  Serial.println("WiFi OK\n");
}

void loop() {

  // LED
  digitalWrite(ledPin, HIGH);
  Serial.println("LED ON");
  delay(500);

  digitalWrite(ledPin, LOW);
  Serial.println("LED OFF");
  delay(500);

  // Sensores
  Serial.print("Temp: ");
  Serial.println(temperatureRead());

  Serial.print("Hall: ");
  Serial.println(hallRead());

  Serial.print("ADC: ");
  Serial.println(analogRead(34));

  Serial.print("Heap: ");
  Serial.println(ESP.getFreeHeap());

  // WiFi scan UMA VEZ
  if (!wifiDone) {
    wifiDone = true;

    Serial.println("\nScan WiFi...");
    delay(1000); // importante

    int n = WiFi.scanNetworks();

    Serial.print("Redes: ");
    Serial.println(n);

    for (int i = 0; i < n; i++) {
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.println(")");
      delay(50); // evita travamento
    }

    Serial.println("\nWiFi OK\n");
  }

  Serial.println("------\n");

  delay(3000); // ESSENCIAL pro watchdog
}
