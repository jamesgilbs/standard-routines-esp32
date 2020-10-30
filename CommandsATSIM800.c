//Rotina para envio de comandos AT para modem SIM800L via monitor serial

/* Comandos mais usados
AT+CLTS=1 Salvar no modem hora e data da rede
AT+CCLK? Retornar informação do RTC interno sobre hora e data
AT+CIPGSMLOC=1,1 Localização
AT+CLBSCFG=1,3,"lbs-simcom.com:3002" Localização
AT+CMGF=1 Ativa a função de envio de SMS no modo texto
AT+CMGS="+55XXXXXXXXXX" Define o número do destinatário
AT+CSQ Testa a intensidade do sinal na recepção
AT+CFUN Verifica o status de funcionalidade
AT+CREG Verifica a informação de registro na rede
AT+CGATT=1 Habilita o serviço de internet do chip de celular
AT+CIPSEND Envia os dados
AT+CIICR Solicita o IP do CHIP
AT+CIPMUX=0 Ativa o modo texto
AT+CREG Informações sobre o status de registro e a tecnologia de acesso da célula servidora
AT+SAPBR=3,1,"Contype","GPRS" Define o tipo de conexão para GPRS
AT+SAPBR=3,1,"APN","timbrasil.br" Vai conectar com a rede de telefonia celular
AT+SAPBR =1,1 Ativa o GPRS (Só precisa ser executado uma vez)
AT+SAPBR=2,1 Consulta se recebeu IP
AT+HTTPINIT Ativa o modo HTTP (Só precisa ser executado uma vez)
AT+HTTPPARA="CID",1 Configura o identificador de perfil HTTP
AT+HTTPPARA="URL","http://api.thingspeak.com/channels/44232/feed/last.json" Define a URL que iremos acessar
AT+HTTPACTION=0 Inicia o download de dados via HTTP GET
AT+HTTPREAD Printa os dados lidos da via HTTP GET
*/

#define TINY_GSM_MODEM_SIM800 //Tipo de modem que estamos usando
#include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM.git versao 0.10.8
#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI.git 2.2.14
#include <SPI.h>

//Canal serial que vamos usar para comunicarmos com o modem. Utilize sempre 1
HardwareSerial SerialGSM(1); //Serial 0 é a serial padrão do ESP via USB
TinyGsm modemGSM(SerialGSM);
TinyGsmClient gsmClient(modemGSM);

TFT_eSPI display = TFT_eSPI();

void setup(){
  //Terminal para debug
  Serial.begin(115200);
  while (!Serial);

  //Comunicacao com o SIM800L
  SerialGSM.begin(9600, SERIAL_8N1, 4, 2, false);
  delay(3000);
  
  setupDisplay(); //Inicializa e configura o display
  delay(2000);
  display.fillScreen(TFT_BLUE);
  display.setCursor(0, 0);

  Serial.println("Debugging SIM800L!");
  display.println("Debugging SIM800L!");
  Serial.println(modemGSM.getModemInfo());
  display.println(modemGSM.getModemInfo());
  display.println("Comando AT Terninal");
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

void loop(){
  //Envio de comandos AT para o modulo
  if (Serial.available()){
    SerialGSM.write(Serial.read());
  }

  //Print dos dados recebidos pelo modulo
  if (SerialGSM.available()){
    Serial.write(SerialGSM.read());
  }
}
