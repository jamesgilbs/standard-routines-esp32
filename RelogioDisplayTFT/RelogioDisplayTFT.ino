#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI.git versao 2.2.14
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

uint32_t targetTime = 0; //para o próximo tempo limite de 1 segundo

byte omm = 99;
boolean initial = 1;
byte xcolon = 0;
unsigned int colour = 0;

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6); //Obtem H, M, S da compilação do tempo

void setup(void) {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK); //Nota: as novas fontes não desenham a cor de fundo

  targetTime = millis() + 1000; 
}

void loop() {
  if (targetTime < millis()) {
    targetTime = millis()+1000;
    ss++;              //Avança segundo
    if (ss==60) {
      ss=0;
      omm = mm;
      mm++;            //Avança minuto
      if(mm>59) {
        mm=0;
        hh++;          //Avança hora
        if (hh>23) {
          hh=0;
        }
      }
    }

    if (ss==0 || initial) {
      initial = 0;
      tft.setTextColor(0xF81F, TFT_BLACK);
      tft.setCursor (52, 52);
      //tft.print(__DATE__); //Isso usa a fonte pequena padrão ADAFruit

      //tft.setTextColor(TFT_BLUE, TFT_BLACK);
      //tft.drawCentreString("It is windy",120,48,2); //Próximo tamanho da fonte 2

      //tft.setTextColor(0xF81F, TFT_BLACK); //Rosa
      //tft.drawCentreString("12.34",80,100,6); //A fonte grande 6 contém apenas caracteres [espaço] 0 1 2 3 4 5 6 7 8 9. : a p m
    }

    //Atualizar hora digital
    byte xpos = 6;
    byte ypos = 0;
    if (omm != mm) { //Apenas redesenhe a cada minuto para minimizar a oscilação
      //Remova o comentário de UMA das próximas 2 linhas, usando a imagem fantasma para demonstrar a sobreposição de texto conforme o tempo passa sobre ela
      tft.setTextColor(0x39C4, TFT_BLACK);  //Deixe uma imagem fantasma de 7 segmentos, comente a próxima linha!
      //tft.setTextColor(TFT_BLACK, TFT_BLACK); //Defina a cor da fonte para preto para limpar a imagem
      //A fonte 7 é para mostrar uma exibição de pseudo 7 segmentos.
      //A fonte 7 contém apenas caracteres [espaço] 0 1 2 3 4 5 6 7 8 9 0:.
      tft.drawString("88:88",xpos,ypos,7); //Substitua o texto para limpá-lo
      tft.setTextColor(TFT_WHITE, TFT_BLACK); //Laranja 0xFBE0
      omm = mm;

      if (hh<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      xpos+= tft.drawNumber(hh,xpos,ypos,7);
      xcolon=xpos;
      xpos+= tft.drawChar(':',xpos,ypos,7);
      if (mm<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      tft.drawNumber(mm,xpos,ypos,7);
    }

    if (ss%2) { // Flash the colon
      tft.setTextColor(0x39C4, TFT_BLACK);
      xpos+= tft.drawChar(':',xcolon,ypos,7);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    else {
      tft.drawChar(':',xcolon,ypos,7);
      colour = random(0xFFFF);
      //Apague o texto antigo com um retângulo, a desvantagem desse método é o aumento da cintilação da tela
      tft.fillRect (0, 64, 160, 20, TFT_BLACK);
      tft.setTextColor(colour);
      tft.drawRightString("Cor: ",75,64,4); //Desenho de string justificado à direita na posição x 75
      String scolour = String(colour,HEX);
      scolour.toUpperCase();
      char buffer[20];
      scolour.toCharArray(buffer,20);
      tft.drawString(buffer,82,64,4);
    }
  }
}
