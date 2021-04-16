/*Coleta sinais da ADC0 e ADC3 formata em dois valores e grava em um arquivo SPIFFS na flash 
 * de 2,5s em 2,5s mostrando em display o valor e a situação da memória.
 * Formata valor com 3 casas de inteiro e duas casas decimais.
 * buttonPin porta 27 como entrada para deletar arquivo criado.
*/

#include <Arduino.h> //Lib Arduino (opcional)
#include <Adafruit_GFX.h> //https://github.com/adafruit/Adafruit-GFX-Library.git versao 1.10.0
#include <Adafruit_ST7735.h> //https://github.com/adafruit/Adafruit-ST7735-Library.git versao 1.6.0
#include <SPI.h> //Comunicação SPI
#include <Fonts/FreeSerif9pt7b.h> //Fonte Serif que é usada no display
#include <SimpleDHT.h> //https://github.com/winlinvip/SimpleDHT.git versao 1.0.12
#include "FS_File_Record.h"  //Nossa lib personalizada do SPIFFS

//Pinos do display
#define DISPLAY_DC 5 //A0 Data command
#define DISPLAY_CS 22 //CS
#define DISPLAY_MOSI 23 //SDA
#define DISPLAY_CLK 18 //SCK
#define DISPLAY_RST 21

// Pino do botão de exclusão de arquivo
const int buttonPin = 27;

// Tamanho dos registros de temperatura e umidade (13 caracteres), exemplo:
// 100.00;100.00
// temp;umid
const int sizeOfRecord = 13;

//100.00
// Variáveis usadas em 'formatValue'
const int integralPartSize = 3, decimalPartSize = 2;

// Objeto da nossa lib que recebe o nome do arquivo e tamanho fixo de registro
FS_File_Record ObjFS("/dht22.bin", sizeOfRecord);

// Pino do DHT22 / Sinais Analógicos
const int pinDHT22 = 32;
float PinA0 = A0;
float PinA3 = A3;
float Temp;
float Gas;

// Objeto do dht22
SimpleDHT22 dht22(pinDHT22);

// Objeto do display
Adafruit_ST7735 display = Adafruit_ST7735(DISPLAY_CS, DISPLAY_DC, DISPLAY_MOSI, DISPLAY_CLK, DISPLAY_RST);

// Altura da fonte, usada na função resetDisplay
int fontHeight = 7;

// Variáveis usada para contagem/comparação de tempo (millis) na função timeout
long millisRefShowSpace;
bool flagShowSpace = false;

// String que recebe as mensagens de erro
String errorMsg;

// Variável que guarda o último registro obtido
String lastRecord = "";

// Existem duas tasks compartilhando o mesmo objeto do display
// Essas flags são usadas para evitar que as duas acessam este objeto ao mesmo tempo
bool flagDisplayisBusy = false, eventButton = false;

// Limpa o display e posiciona o cursor no início
void resetDisplay()
{
  // Verificamos se o display não está ocupado por alguma das tasks
  if(!flagDisplayisBusy)
  {
    flagDisplayisBusy = true;  
    display.setFont(&FreeSerif9pt7b);
    display.fillScreen(ST77XX_BLACK);
    display.setTextColor(ST7735_WHITE);
    display.setCursor(0,fontHeight+5);
    display.setTextSize(1);
    flagDisplayisBusy = false; 
  }
}

void showDisplay(String msg)
{
  // Verificamos se o display não está ocupado por alguma das tasks
  if(!flagDisplayisBusy)
  {    
    flagDisplayisBusy = true;  // Sinalizamos que o display está ocupado
    display.println(msg);          // Exibimos no display a mensagem
    flagDisplayisBusy = false; // Sinalizamos que o display está desocupado
  }   
}

// Posiciona cursor no centro e exibe a mensagem "FILE DELETED" em amarelo
void showFileDeleted(bool sucess)
{
  // Posição y aonde o texto será exibido
  int y = (display.height()/2)-(fontHeight*2);

  // Limpamos o display (aqui não usamos a função resetDisplay porque a flag displayisBusy neste momento está como true e o resetDisplay não será executado)
  // Portanto limpamos o display executando os comandos separadamente
  display.setFont(&FreeSerif9pt7b);
  display.fillScreen(ST77XX_BLACK);
  display.setTextColor(ST7735_WHITE);
  display.setTextSize(1);
  
  // Define cor do texto amarela
  display.setTextColor(ST7735_YELLOW);
  // Posiciona no centro de eixo y
  display.setCursor(0, y);    

  // Se foi possível excluir
  if(sucess)
  {
    // Exibe mensagem "FILE DELETED"
    display.println("   Arquivo");    
    display.println("   Deletado!");
  }
  else // Se não foi possível excluir
  {
    // Exibe mensagem "CANNOT DELETE"
    display.println("   Não");
    display.println("   Deletado");
  }

  // Define cor do texto branca
  display.setTextColor(ST7735_WHITE);
}

// Exibimos no display o arquivo, pausando a exibição a cada 6 registros
void showFile()
{
  int count = 0;
  String linha = "";  
  
  // Exibe na serial e no display o início do arquivo
  Serial.println("# Inicio Arquivo #");  
  showDisplay("#Inicio Arquivo#");

  errorMsg = "";

  // Posiciona o ponteiro do arquivo no início
  ObjFS.rewind();

  // Exibe todos os registros até o fim
  while(ObjFS.readFileNextRecord(&linha, &errorMsg) && linha != "")
  {
    Serial.println(linha);
    count++;

    // A cada 6 registros, pausamos e resetamos o display
    if(count % 6 == 0)
    {
      //Exibe "..." sinalizando que ainda não é o fim do arquivo
      showDisplay("...");
      // Aguarda 1.5s para poder visualizar os valores
      delay(1500);
      // Limpa display
      resetDisplay();
    }
    showDisplay(linha);
  }
    
  // Se existir mensagem de erro exibe na serial e no display
  if(errorMsg != "")
  {
    Serial.println(errorMsg);
    showDisplay(errorMsg);
  }

  // Exibe na serial e no display o fim do arquivo
  Serial.println("# Fim Arquivo #");   
  showDisplay("# Fim Arquivo #");
  delay(1500);
}

void setup() 
{
  // Inicializamos o display
  display.initR(INITR_BLACKTAB);

  // Iniciamos a Serial com velocidade de 115200
  Serial.begin(115200);
  
  // Seta botão como entrada (INPUT)
  pinMode(buttonPin, INPUT);  

  // Exibe na serial "Starting..." para debug
  Serial.print("Iniciando...");

  // Se não foi possível iniciar o File System, exibimos erro e reiniciamos o ESP
  if(!ObjFS.init())
  {
    Serial.println("Erro Arquivo de Sistema");
    delay(1000);
    ESP.restart();
  }
  
  // Exibimos mensagem
  Serial.println("Arquivo de Sistema ok");
  
  // Se o arquivo não existe, criamos o arquivo
  if(!ObjFS.fileExists())
  {
     Serial.println("Novo Arquivo");
     ObjFS.newFile(); // Cria o arquivo
  }  

  // Iniciamos uma task que irá ler o botão de exclusão
 xTaskCreatePinnedToCore(
  buttonEvent,   //Função que será executada
  "buttonEvent", //Nome da tarefa
  10000,     //Tamanho da pilha
  &display,      //Parâmetro da tarefa (no caso não usamos)
  2,         //Prioridade da tarefa
  NULL,      //Caso queira manter uma referência para a tarefa que vai ser criada (no caso não precisamos)
  0);        //Número do core que será executada a tarefa (usamos o core 0 para o loop ficar livre com o core 1)
  
  
  resetDisplay();

  // EXEMPLO DE BUSCA (FIND)
  // Obs: O primeiro registro se posiciona na pos 0 (zero) 
  // String reg = ObjFS.findRecord(10);
  // showDisplay(reg);  

  // Exibimos o arquivo
  showFile();   
}

// Formata valores colocando zero à esquerda, ex: 023.20;047.40 
String formatValue(float value)
{
  char auxDecimalPart[decimalPartSize+1], integralPart[integralPartSize+1], buf[sizeOfRecord+1]; //+1 para o '\0'
  String decimalPart;

  char format[5];

  // Copiamos para a formatação da parte decimal o texto: %0.2f
  strcpy(format, "%.");
  strcat(format, String(decimalPartSize).c_str());
  strcat(format, "f");
 
  sprintf (auxDecimalPart, format, value-(int)value);

  // Atribuímos para a variável decimalPart a parte decimal considerando os zeros à direita 
  decimalPart = String(auxDecimalPart);
  decimalPart.replace("0.","");  

  // Copiamos para a formatação da parte inteira o texto: %03d
  strcpy(format, "%0");
  strcat(format, String(integralPartSize).c_str());
  strcat(format, "d");

  // Atribuímos para a variável integralPart a parte inteira considerando os zeros à esquerda  
  sprintf (integralPart, format, (int)value);

  // Concatenamos a parte interia com a parte decimal separando com ponto
  sprintf (buf, "%s.%s", integralPart, decimalPart.c_str());

  return String(buf);
}

// Função que lê e formata os dados de temperatura e umidade
String readDHTValues()
{
  Temp = analogRead(PinA3)*0.1;//°C
  Gas = analogRead(PinA0)/2.5;//ppm
  //float temperature = 0;
  //float humidity = 0;
  int err = SimpleDHTErrSuccess;

  // Lê sensor
  /*if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) 
  {
    Serial.print("Leitura DHT Falhou, erro = "); 
    Serial.println(err);
    delay(1000);
    return "-------------"; //importante: deve possuir o mesmo sizeOfRecord, ou seja, 13 caracteres
  }*/

  // Retorna valores formatados, separados por ponto-virgula
  return formatValue(Temp)+"/"+formatValue(Gas);
}

//Função que compara se o tempo foi atingido, sem travar o loop
bool timeout(const int DELAY, long *millisAnterior, bool *flag)
{
  if(*flag)
  {
    *millisAnterior = millis();
    *flag = false;
  }  

  if((*millisAnterior + DELAY) < millis())
  {
    *flag = true;
    return true;
  }

  return false;
}

// Exibe o espaço total, usado e disponível no display
void showAvailableSpace()
{
  long prevMillis;
  if(!eventButton && timeout(10000, &millisRefShowSpace, &flagShowSpace))
  {
    Serial.println("Space: "+String(ObjFS.getTotalSpace())+" Bytes");
    Serial.println("Used: "+String(ObjFS.getUsedSpace())+" Bytes");
    resetDisplay();
    showDisplay("Espaco Total:");
    showDisplay(String(ObjFS.getTotalSpace())+" Bytes");
    display.setTextColor(ST7735_YELLOW);
    showDisplay("Espaco Usado:");
    showDisplay(String(ObjFS.getUsedSpace())+" Bytes");
    display.setTextColor(ST77XX_GREEN);
    showDisplay("Espaco Livre:");
    showDisplay(String(ObjFS.getTotalSpace()-ObjFS.getUsedSpace())+" Bytes");

    // Registramos 4 vezes enquanto a mensagem aparece no display
    for(int i=0; i<4; i++) 
    {
      // Lê temperatura e umidade do DHT
      String values = readDHTValues();

      // Escrevemos no arquivo e exibimos erro ou sucesso na serial para debug
      if(values != "" &&!ObjFS.writeFile(values, &errorMsg))
        Serial.println(errorMsg);
      else
        Serial.println("Escrita ok");

      prevMillis = millis();

      while(prevMillis+2500 > millis() && !eventButton);

      if(eventButton)     
      {
        eventButton = false;
        // aguarda exibição "FILE DELETED"
        delay(500);
        break;
      } 
    } //fim do for
  } //fim do if(timeout)
} //fim da função

// Exibe última amostra de temperatura e umidade obtida
void showLastRecord()
{
  resetDisplay();
  showDisplay("Temp / Gas:");
  showDisplay(lastRecord);
}

void loop() 
{  
  // Se não houver memória disponível, exibe e reinicia o ESP
  if(!ObjFS.availableSpace())
  {
    Serial.println("Memória Cheia!");
    showDisplay("Memória Cheia!");
    delay(10000);
    return;
  }

  // Lê temperatura e umidade do DHT
  String values = readDHTValues();

  // Escrevemos no arquivo e exibimos erro ou sucesso na serial para debug
  if(values != "" && !ObjFS.writeFile(values, &errorMsg))
    Serial.println(errorMsg);
  else
    Serial.println("Escrita ok");

  // Atribui para a variável global a última amostra
  lastRecord = values;
  // Exibe a última amostra no display
  showLastRecord();
  // Exibimos o espaço total, usado e disponível no display, de tempo em tempo
  showAvailableSpace();
  
  // Aguarda 2500ms
  delay(2500);
}

// Função executada pela task de evento do botão
void buttonEvent(void* display)
{   
  TickType_t taskDelay = 10 / portTICK_PERIOD_MS;

  // IMPORTANTE: A tarefa não pode terminar, deve ficar presa em um loop infinito
  while(true)
  {
    // Se o botão foi pressionado e a flag estiver "false"
    if(digitalRead(buttonPin) == HIGH && !eventButton)
    {    
      // Sinalizamos que o botão foi pressionado
      eventButton = true;
      // Tenta excluir o arquivo
      if(ObjFS.destroyFile())     
      {
        Serial.println("Arquivo deletado");

        // Enquanto o display estiver ocupado, aguardamos
        while(flagDisplayisBusy)
          vTaskDelay(taskDelay);

        // Sinalizamos que o display está ocupado
        flagDisplayisBusy = true;
        // Exibimos no display
        showFileDeleted(true);
        // Sinalizamos que o display está desocupado
        flagDisplayisBusy = false;

        lastRecord = "";
      }
      else
      {
        Serial.println("Falha ao deletar arquivo");
        
        while(flagDisplayisBusy)
          vTaskDelay(taskDelay);

        // Sinalizamos que o display está ocupado
        flagDisplayisBusy = true;
        // Exibimos no display
        showFileDeleted(false);
        // Sinalizamos que o display está desocupado
        flagDisplayisBusy = false;
      }
    }
    else
    if(digitalRead(buttonPin) == LOW && eventButton) // Se o botão foi solto e a flag estiver "true"
    { 
      // Sinalizamos que o botão foi solto
      eventButton = false;
    }  
    
    // Executamos um delay de 10ms, os delays executado nas xTasks são diferentes
    vTaskDelay(taskDelay);

    //IMPORTANTE: SEMPRE DEIXAR UM DELAY PARA ALIMENTAR WATCHDOG
  }
}
