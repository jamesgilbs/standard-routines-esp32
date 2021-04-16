#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI.git 2.2.14
/*
 *User_Setup.h
 *#define TFT_MISO 19
 *#define TFT_MOSI 23
 *#define TFT_SCLK 18
 *#define TFT_CS   22  // Chip select control pin
 *#define TFT_DC   5  // Data Command control pin
 *#define TFT_RST   21  // Reset pin (could connect to RST pin)
*/

#include <SPI.h>

TFT_eSPI display = TFT_eSPI();
//LED interno (pode variar o pino de placa para placa)
const int LED = 2;
//limite para ativar o LED
const int LIMIT = 0;

void setup()
{
  Serial.begin(115200);
  //configura o pino do LED como saída
  pinMode(LED, OUTPUT);
  setupDisplay(); //Inicializa e configura o display
  delay(2000);
  display.fillScreen(TFT_BLUE);
  display.setCursor(0, 0);
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

void loop()
{
   //guarda o valor lido do sensor hall
   int measurement = 0;
   //faz a leitura do sensor hall
   measurement = hallRead();
   Serial.print("Campo Magnetico: ");//envia mensagem para a serial
   Serial.println(measurement);
   display.print("Campo Magnetico: ");//envia mensagem para o display
   display.println(measurement);
   //verifica se o valor lido é menor que o limite definido
   if(measurement < LIMIT) {
      //liga o LED
      digitalWrite(LED, HIGH);
   }
   else   {
      //desliga o LED
      digitalWrite(LED, LOW);
   }
   delay(50);
}
