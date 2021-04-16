#define TINY_GSM_MODEM_SIM800 //Type of modem we are using
#include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM.git versao 0.10.8

//Serial channel that we will use to communicate with the modem. Always use 1
HardwareSerial SerialGSM(1);
//gsmClient is the outgoing information client
TinyGsm modemGSM(SerialGSM);
TinyGsmClient gsmClient(modemGSM);

void setup() {
  //We connect to GSM SIM800
  setupGSM();
}

void setupGSM()
{
  Serial.println("Setup GSM...");

  //Initialize the serial where the modem is
  SerialGSM.begin(9600, SERIAL_8N1, 4, 2, false);
  delay(3000);

  //Shows information about the modem
  Serial.println(modemGSM.getModemInfo());

  //Initializes the modem
  if (!modemGSM.restart())
  {
    Serial.println("Reset, GSM fail");
    delay(10000);
    ESP.restart();
    return;
  }
  //Whating for network
  if (!modemGSM.waitForNetwork())
  {
    Serial.println("Fail GSM network");
    delay(10000);
    ESP.restart();
    return;
  }

  //Connect to the gprs network (APN, user, password)
  if (!modemGSM.gprsConnect("", "", "")) {
    Serial.println("GPRS fial");
    delay(10000);
    ESP.restart();
    return;
  }
  Serial.println("GSM conected");
}

void loop() {
}
