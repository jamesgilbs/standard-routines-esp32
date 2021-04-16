#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI.git versao 2.2.14
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(); //Biblioteca de chamadas, pinos definidos em User_Setup.h

#define TFT_GREY 0xBDF7

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0; //Multiplicadores salvos de H, M, S x e y
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=64, osy=64, omx=64, omy=64, ohx=64, ohy=64; //Coords H, M, S x e y salvos
uint16_t x0=0, x1=0, yy0=0, yy1=0;
uint32_t targetTime = 0; //para o próximo tempo limite de 1 segundo

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6); //Obtenha H, M, S do tempo de compilação

boolean initial = 1;

void setup(void) {
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_GREY);
  tft.setTextColor(TFT_GREEN, TFT_GREY); //Adicionar uma cor de fundo preta apaga o texto anterior automaticamente
  
  // Draw clock face
  tft.fillCircle(64, 64, 61, TFT_BLUE);
  tft.fillCircle(64, 64, 57, TFT_BLACK);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*57+64;
    yy0 = sy*57+64;
    x1 = sx*50+64;
    yy1 = sy*50+64;

    tft.drawLine(x0, yy0, x1, yy1, TFT_BLUE);
  }

  //Desenha 60 pontos
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*53+64;
    yy0 = sy*53+64;
    
    tft.drawPixel(x0, yy0, TFT_BLUE);
    if(i==0 || i==180) tft.fillCircle(x0, yy0, 1, TFT_CYAN);
    if(i==0 || i==180) tft.fillCircle(x0+1, yy0, 1, TFT_CYAN);
    if(i==90 || i==270) tft.fillCircle(x0, yy0, 1, TFT_CYAN);
    if(i==90 || i==270) tft.fillCircle(x0+1, yy0, 1, TFT_CYAN);
  }

  tft.fillCircle(65, 65, 3, TFT_RED);

  //Desenhe texto na posição 64.125 usando fontes 4
  //Apenas os números de fonte 2,4,6,7 são válidos. A fonte 6 contém apenas caracteres [espaço] 0 1 2 3 4 5 6 7 8 9:. a p m
  //A fonte 7 é uma fonte de 7 segmentos e contém apenas caracteres [espaço] 0 1 2 3 4 5 6 7 8 9:.
  tft.drawCentreString("RELOGIO",64,130,4);

  targetTime = millis() + 1000; 
}

void loop() {
  if (targetTime < millis()) {
    targetTime = millis()+1000;
    ss++;              //Avança segundos
    if (ss==60) {
      ss=0;
      mm++;            //Avança minutos
      if(mm>59) {
        mm=0;
        hh++;          //Avança horas
        if (hh>23) {
          hh=0;
        }
      }
    }

    //Pré-calcule os graus manuais, coordenadas x e y para uma atualização rápida da tela
    sdeg = ss*6;                  // 0-59 -> 0-354
    mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - inclui segundos
    hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - inclui minutos e segundos
    hx = cos((hdeg-90)*0.0174532925);    
    hy = sin((hdeg-90)*0.0174532925);
    mx = cos((mdeg-90)*0.0174532925);    
    my = sin((mdeg-90)*0.0174532925);
    sx = cos((sdeg-90)*0.0174532925);    
    sy = sin((sdeg-90)*0.0174532925);

    if (ss==0 || initial) {
      initial = 0;
      //Apaga as posições dos ponteiros das horas e minutos a cada minuto
      tft.drawLine(ohx, ohy, 65, 65, TFT_BLACK);
      ohx = hx*33+65;    
      ohy = hy*33+65;
      tft.drawLine(omx, omy, 65, 65, TFT_BLACK);
      omx = mx*44+65;    
      omy = my*44+65;
    }

      //Redesenha as novas posições dos ponteiros, ponteiros das horas e minutos não apagados aqui para evitar tremulação
      tft.drawLine(osx, osy, 65, 65, TFT_BLACK);
      tft.drawLine(ohx, ohy, 65, 65, TFT_WHITE);
      tft.drawLine(omx, omy, 65, 65, TFT_WHITE);
      osx = sx*47+65;    
      osy = sy*47+65;
      tft.drawLine(osx, osy, 65, 65, TFT_RED);

    tft.fillCircle(65, 65, 3, TFT_RED);
  }
}
