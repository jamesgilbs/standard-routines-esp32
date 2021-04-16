//Libs de WiFi, MQTT e Json
#include <WiFi.h> //https://www.arduino.cc/en/Reference/WiFi versao 1.2.6
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient.git versao 2.7.0
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson.git versao 5.13.5

//needed for library
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal ( https://github.com/zhouhan0126/DNSServer---esp32 )
#include <WebServer.h> //Local WebServer used to serve the configuration portal ( https://github.com/zhouhan0126/WebServer-esp32 )
#include <WiFiManager.h>   //https://github.com/zhouhan0126/WIFIMANAGER-ESP32

//Substitua pelo SSID da sua rede
#define SSID "TP-LINK_DF56FC"

//Substitua pela senha da sua rede
#define PASSWORD "86586962"


//Altere estes dados para os obtidos quando se cadastra um novo dispositivo
//no site do Watson IoT
const String ORG = "kz9i2l";
const String DEVICE_TYPE = "ESP32";
const String DEVICE_ID = "001";
#define DEVICE_TOKEN "?AgVV7!jFINxNg9D4T"


//ID que usaremos para conectar 
const String CLIENT_ID = "d:"+ORG+":"+DEVICE_TYPE+":"+DEVICE_ID;

//Server MQTT que iremos utilizar
const String MQTT_SERVER = ORG + ".messaging.internetofthings.ibmcloud.com";

//Comandos para o Relê 1
#define COMMAND_TOPIC_R1 "iot-2/cmd/commandR1/fmt/json"

//Comandos para o Relê 2
#define COMMAND_TOPIC_R2 "iot-2/cmd/commandR2/fmt/json"

//Pinos dos relês
#define R1_PIN 25
#define R2_PIN 26
#define CONNECT_PIN 12

//Cliente WiFi que o MQTT irá utilizar para se conectar
WiFiClient wifiClient;

//Cliente MQTT, passamos a url do server, a porta
//e o cliente WiFi
PubSubClient client(MQTT_SERVER.c_str(), 1883, wifiClient);

/*//quando utilizado o portal para se conectar a uma rede, esse boleano nos indica que 
bool shouldSaveConfig = false;
String ssid = "";
String pwd = "";*/

void setup() {
    Serial.begin(115200);

    //Coloca os pinos dos relês como saída
    pinMode(R1_PIN, OUTPUT);
    pinMode(R2_PIN, OUTPUT);
    pinMode(CONNECT_PIN, OUTPUT);

    digitalWrite(CONNECT_PIN, LOW);

    //declaração do objeto wifiManager
    WiFiManager wifiManager;

    //callback para quando entra em modo de configuração AP
    wifiManager.setAPCallback(configModeCallback); 
    //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
    wifiManager.setSaveConfigCallback(saveConfigCallback); 

    //caso queira iniciar o Portal para se conectar a uma rede toda vez, sem tentar conectar 
    //a uma rede salva anteriormente, use o startConfigPortal em vez do autoConnect
    //wifiManager.startConfigPortal("ESP_AP");


    //IP DO PORTAL --> 192.168.4.1
    wifiManager.autoConnect("ESP_AP"); //cria uma rede sem senha

    //Conectamos à rede WiFi
    //setupWiFi();
    //Conectamos ao server MQTT
    connectMQTTServer();
}

void loop() {
    //Verifica se existe alguma mensagem em algum tópico
    //que seguimos
    client.loop();
}

//Função responsável por conectar ao server MQTT
void connectMQTTServer() {
  Serial.println("Connecting to MQTT Server...");
  //Se conecta com as credenciais obtidas no site do Watson IoT
  //quando cadastramos um novo device
  if (client.connect(CLIENT_ID.c_str(), "use-token-auth", DEVICE_TOKEN)) {
    //Se a conexão foi bem sucedida
    Serial.println("Connected to MQTT Broker");
    //Quando algo for postado em algum tópico que estamos inscritos
    //a função "callback" será executada
    client.setCallback(callback);
    //Se inscreve nos tópicos de interesse
    client.subscribe(COMMAND_TOPIC_R1);
    client.subscribe(COMMAND_TOPIC_R2);
    digitalWrite(CONNECT_PIN, HIGH);
  } else {
    //Se ocorreu algum erro
    Serial.print("error = ");
    Serial.println(client.state());
    connectMQTTServer(); //tenta conectar novamente
  }
  
}

void callback(char* topic, unsigned char* payload, unsigned int length) {
    Serial.print("topic ");
    Serial.println(topic);
    //Faz o parse do payload para um objeto json
    StaticJsonBuffer<30> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(payload);

    //Se não conseguiu fazer o parser
    if(!root.success())
    {
        //Exibe mensagem de erro
        Serial.println("Json Parse Error");
        return;
    }

    //Recupera o atributo "value" do json
    int value = root["value"];

    //Verifica se a string topic é igual ao que está em  COMMAND_TOPIC_R1
    if (strcmp(topic, COMMAND_TOPIC_R1) == 0)
    {
        //Modifica o valor de saída do pino onde está o relê 1 para o que nos foi enviado
        digitalWrite(R1_PIN, value);
    }
    //senão, como estamos inscritos em apenas 2 tópicos, significa que é do tópico que sobrou
    else
    {
        //Modifica o valor de saída do pino one está o relê 2 para o que nos foi enviado
        digitalWrite(R2_PIN, value);
    }
}


/*WIFIMANAGER*/
//callback que indica que o ESP entrou no modo AP
void configModeCallback (WiFiManager *myWiFiManager) {  
//  Serial.println("Entered config mode");
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede

}

//callback notifying us of the need to save config
//callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
//  Serial.println("Should save config");
  Serial.println("Configuração salva");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
}
