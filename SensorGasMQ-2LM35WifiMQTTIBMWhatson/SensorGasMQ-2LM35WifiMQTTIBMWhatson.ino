//Projeto lê os dados na porta analógica ADC0 e ADC3 e envia via Wifi e MQTT para
//https://quickstart.internetofthings.ibmcloud.com/#/device/SensorGasESP32/sensor/

//Leitura do ADC0 sensor de gás MQ-2
//sinal analógico do sensor MQ-2 GPIO36 ADC0 SensVP ESP32 30 pin
//Leitura do ADC3 sensor de temperatura LM35
//sinal analógico do sensor LM35 GPIO34 ADC0 SensVN ESP32 30 pin

//Verifica qual ESP está sendo utilizado
//e importa a lib e wifi correspondente
#if defined(ESP8266)
#include <ESP8266WiFi.h>        
#else
#include <WiFi.h> //https://www.arduino.cc/en/Reference/WiFi versao 1.2.6
#endif

#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI.git versao 2.2.14
#include <SPI.h>

//Pinagem em User_Setup.h na pasta da biblioteca
TFT_eSPI display = TFT_eSPI();

//Lib de MQTT
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient.git versao 2.7.0
//Lib do sensor de temperatura e umidade
//#include <DHT.h> //https://github.com/adafruit/DHT-sensor-library.git versao 1.3.0

//Intervalo entre os envios
#define INTERVAL 1000

//SSID da rede
#define SSID "TP-LINK_DF56FC"

//Senha da sua rede
#define PASSWORD "86586962"

//Server MQTT que iremos utlizar
#define MQTT_SERVER "quickstart.messaging.internetofthings.ibmcloud.com"

//Nome do tópico que devemos enviar os dados
//para que eles apareçam nos gráficos
#define TOPIC_NAME "iot-2/evt/status/fmt/json"

//ID que usaremos para conectar 
//QUICK_START deve permanecer como está
const String QUICK_START = "d:quickstart:arduino:";

//No DEVICE_ID você deve mudar para um id único
//Aqui nesse exemplo utilizamos o MAC Address
//do dispositivo que estamos utilizando
//Servirá como identificação no site
//https://quickstart.internetofthings.ibmcloud.com
const String DEVICE_ID = "SensorGasESP32";

//Concatemos o id do quickstart com o id do nosso
//dispositivo
const String CLIENT_ID =  QUICK_START + DEVICE_ID;

//Cliente WiFi que o MQTT irá utilizar para se conectar
WiFiClient wifiClient;

//Cliente MQTT, passamos a url do server, a porta
//e o cliente WiFi
PubSubClient client(MQTT_SERVER, 1883, wifiClient);

//Tempo em que o último envio foi feito
long lastPublishTime = 0;

//Leitura do ADC0 sensor de gás MQ-2
float PinA0 = A0;//sinal analógico do sensor MQ-2 GPIO36 ADC0 SensVP ESP32 30 pin
float leitura;
float ppm;

//Leitura do ADC3 sensor de temperatura LM35
float PinA3 = A3; //sinal analógico do sensor LM35 GPIO34 ADC0 SensVN ESP32 30 pin
float temperatura;

//Objeto que realiza a leitura da temperatura e da umidade
//DHT dht(4, DHT22);

//Variável para guardarmos o valor da temperatura
//float temperature = 0;

//Variável para guardarmos o valor da umidade
//float humidity = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(PinA0, INPUT);
  
  //Incializamos o dht
  //dht.begin();
  
  setupDisplay(); //Inicializa e configura o display
  delay(2000);
  display.fillScreen(TFT_BLUE);
  display.setCursor(0, 0);
  
  //Conectamos à rede WiFi
  setupWiFi();
  
  //Conectamos ao server MQTT
  connectMQTTServer();
}

void setupDisplay()
{
  display.init();
  display.setRotation(1);
  display.fillScreen(TFT_BLUE); //Limpa o display com a cor azul
  display.setTextColor(TFT_WHITE, TFT_BLUE); //Coloca o texto como branco com fundo azul
  display.setTextWrap(true, true);//Ativa quebra de linha
  display.setTextSize(1);
  display.setCursor(0, 0, 2); //Posicção x, y e fonte do texto
  display.println("Display Ok");
}

void loop() {
  //Tempos agora em milisegundos
  long now = millis();

  //Se o tempo desde o último envio for maior que o intervalo de envio
  if (now - lastPublishTime > INTERVAL) {
    //Atualizamos o tempo em que ocorreu o último envio
    lastPublishTime = now;
    
    //Rotina para leitura dos sinais
    readSensor();
    
    //Criamos o json que enviaremos para o server mqtt
    String msg = createJsonString();
    
    //Publicamos no tópico onde o servidor espera para receber 
    //e gerar o gráfico
    client.publish(TOPIC_NAME, msg.c_str());
    
    Serial.print("Mensagem publicada: ");
    Serial.println(msg);
    //display.print("Mensagem publicada: ");
    //display.println(msg);
  }
}

//Função responsável por conectar à rede WiFi
void setupWiFi() {
  Serial.println();
  display.println();
  Serial.print("Conectando em ");
  Serial.print(SSID);
  display.print("Conectando em ");
  display.print(SSID);
  
  //Manda o esp se conectar à rede através
  //do ssid e senha
  WiFi.begin(SSID, PASSWORD);

  //Espera até que a conexão com a rede seja estabelecida
  while (WiFi.status() != WL_CONNECTED) {
    delay(25);
    Serial.print(".");
    display.print(".");
  }

  //Se chegou aqui é porque conectou
  Serial.println("");
  display.println("");
  Serial.println("WiFi conectado");
  display.println("WiFi conectado");
}

//Função responsável por conectar ao server MQTT
void connectMQTTServer() {
  Serial.println("Conectando ao MQTT...");
  display.println("Conectando ao MQTT...");
  
  //Se conecta ao id que definimos
  if (client.connect(CLIENT_ID.c_str())) {
    //Se a conexão foi bem sucedida
    Serial.println("Conectado ao MQTT");
    display.println("Conectado ao MQTT");
  } else {
    //Se ocorreu algum erro
    Serial.print("Erro = ");
    Serial.println(client.state());
    display.print("Erro = ");
    display.println(client.state());
  }
}

//Função responsável por realizar a leitura da temperatura e umidade DHT22
void readSensor(){
  float valor;
  float valortemp;
  //float value;
  
  //leitura do ADC0 sensor de Gás MQ-2 
  valor = analogRead(PinA0); 
  if(!isnan(valor)){
    //Armazena o novo valor da leitura
    leitura = valor;
    ppm = leitura * 1.979;
    }
  valortemp = analogRead(PinA3); 
  if(!isnan(valortemp)){
    //Armazena o novo valor da leitura
    temperatura = valortemp * 0.1;
    }
    
  /*//Leitura para DHT22 no GPIO4
  //Faz a leitura da temperatura
  value = dht.readTemperature();
  //Se o valor lido é válido
  if(!isnan(value)){
    //Armazena o novo valor da temperatura
    temperature = value;
  }
  //Faz a leitura da umidade
  value = dht.readHumidity();
  //Se o valor for válido
  if(!isnan(value)){
    //Armazena o novo valor da umidade
    humidity = value;
  }*/
}

//Função responsável por criar
//um Json com os dados lidos
String createJsonString() {

  //String do ADC0 
  String data = "{";
    data+= "\"d\": {";
      data+="\"Leitura do Sensor (mV)\":";
      data+=String(leitura);
      data+=",";
      data+="\"Concentração de GLP, Metano, Propano, Butano, Hidrogênio, Álcool, Gás Natural (ppm)\":";
      data+=String(ppm);
      data+=",";
      data+="\"Temperatura Ambiente (ºC)\":";
      data+=String(temperatura);
    data+="}";
   data+="}";
  return data;

  //String do DHT22
  /*String data = "{";
    data+= "\"d\": {";
      data+="\"temperature\":";
      data+=String(temperature);
      data+=",";
      data+="\"humidity\":";
      data+=String(humidity);
    data+="}";
  data+="}";
  return data;*/
}
