void setup()
{
  Serial2.begin(115200);
  pinMode(13, OUTPUT);//LED
  pinMode(8, OUTPUT);//Pino de reset do GSM
  //reinicia o GSM
  digitalWrite(8, 0);
  delay(2000);
  digitalWrite(8, 1);
  delay(7000);
  if (gsmWrite("AT", "OK") == "FAIL")
  {
    return;//Se o GSM nao responder, retorna a funcao (trava o mcu)
  }
  delay(5000);//Espera o termino da inicializacao do GSM
  SMSsnd("014998202683", "Arduino ON, lendo mensagens...");//Envia a mensagem pro numero. Atencao: precisa do DDD!
}
void loop()
{
  SMSrd();//Verifica se há novas mensagens para serem lidas a cada 10seg
  delay(10000);
}
String gsmWrite(String snd, String rcv)//Funcao que envia dados pro GSM e espera a resposta de ate 30seg
{
  Serial2.println(snd);
  if (rcv.indexOf("+CMGS") > -1)
  {
    delay(150);
    Serial2.write(0x1A);
  }
  for (uint16_t i = 0; i < 1200; i++)
  {
    delay(25);
    if (Serial2.available())
    {
      delay(50);
      String a = Serial2.readString();
      if (a.indexOf(rcv) > -1 || rcv.length() == 0)
      {
        return a;
      }
    }
  }
  return "FAIL";
}
void SMSsnd(String nm, String msg)//Funcao que envia o SMS
{
  String snd = "AT+CMGS=\"";  snd += nm;  snd += "\"";
  gsmWrite("AT+CMGF=1", "OK");
  gsmWrite("AT+CSMP=17,255,0,0", "OK");
  gsmWrite(snd, ">");
  gsmWrite(msg, "+CMGS:");
}
void SMSrd()//Funcao que le se ha SMS nao lido
{
  String c = "";
  gsmWrite("AT+CMGF=1", "OK");
  c = gsmWrite("AT+CMGL=\"REC UNREAD\"", "");
  if (c.indexOf("+CMGL:") > -1)
  {
    if    (c.indexOf("ON") > -1)//ON
    {
      digitalWrite(13, 1);//se o SMS conter o texto ON, liga o LED
    }
    else if (c.indexOf("OFF") > -1)//OFF
    {
      digitalWrite(13, 0);//se o SMS conter o texto OFF, desliga o LED
    }
    gsmWrite("AT+CMGD=1,1", "OK");//apaga todas mensagens (SIM card ha pouca memoria)
  }
}





//Programa: Controle de Arduino por SMS com SIM800L
//Autor: Arduino e Cia
#include <Sim800l.h>
#include <SoftwareSerial.h>
Sim800l Sim800l;
String textSms, numberSms;
uint8_t LED1 = 4; //Conexao Led vermelho
uint8_t LED2 = 5; //Conexao Led verde
bool error;
void setup()
{
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Sim800l.begin();
  Serial.print("Limpando SMS antigos...");
  error = Sim800l.delAllSms(); //Apaga SMS
  Serial.println(" Apagados!");
  Serial.println("\nAguardando comandos por SMS...");
}
void loop()
{
  //Le o SMS da posicao 1
  textSms = Sim800l.readSms(1);
  //Verifica se o SMS foi recebido e eh valido
  if (textSms.indexOf("OK") != -1)
  {
    //Verifica se o SMS esta vazio
    if (textSms.length() > 7)
    {
      //Retorna o numero do celular que enviou o SMS
      numberSms = Sim800l.getNumberSms(1);
      //Mostra o numero do telefone no Serial Monitor
      Serial.println(numberSms);
      //Converte o texto para maiusculas
      textSms.toUpperCase();
      //Caso receba L1, acende o led 1 (vermelho)
      if (textSms.indexOf("L1") != -1)
      {
        Serial.println("Led 1 Ligado");
        digitalWrite(LED1, 1);
      }
      //Caso receba D1, apaga o led 1 (vermelho)
      else if (textSms.indexOf("D1") != -1)
      {
        Serial.println("Led 1 Desligado");
        digitalWrite(LED1, 0);
      }
      //Caso receba L2, acende o led 2 (verde)
      else if (textSms.indexOf("L2") != -1)
      {
        Serial.println("Led 2 Ligado");
        digitalWrite(LED2, 1);
      }
      //Caso receba D2, apaga o led 2 (verde)
      else if (textSms.indexOf("D2") != -1)
      {
        Serial.println("Led 2 Desligado");
        digitalWrite(LED2, 0);
      }
      //Apaga todos as mensagens da memoria, garantindo que o proximo
      //SMS esteja sempre na posicao 1
      Sim800l.delAllSms();
    }
  }
}





//Programa: Envio de SMS com SIM800L
//Autor: Arduino e Cia
#include <Sim800l.h>
#include <SoftwareSerial.h>
Sim800l Sim800l;
char* texto = "Sensor acionado!";
char* numero = "04111987654321";
#define pino_sensor A1
int valor;
void setup()
{
  Serial.begin(9600);
  pinMode(pino_sensor, INPUT);
  Sim800l.begin(); // initializate the library.
  Serial.println("Aguardando acionamento do sensor...");
}
void loop()
{
  valor = digitalRead(pino_sensor);
  if (valor != 0)
  {
    Serial.println("Sensor acionado! Enviando SMS...");
    Sim800l.sendSms(numero, texto);
    Serial.println("SMS Enviado...");
    while (digitalRead(pino_sensor) != 0)
    {
      delay(100);
    }
    Serial.println("Aguardando novo acionamento");
  }
}
