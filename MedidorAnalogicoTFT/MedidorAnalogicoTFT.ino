//Medidor tipo agulha da entrada ADC0

// Define meter size
#define M_SIZE 0.667

#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI.git versao 2.2.14
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

#define TFT_GREY 0x5AEB
#define TFT_ORANGE      0xFD20      /* 255, 165,   0 */

int PinA0 = A0;//sinal analógico do sensor MQ-2

float ltx = 0;    //X coord salva da parte inferior da agulha
uint16_t osx = M_SIZE*120, osy = M_SIZE*120; //Coordenadas x e y salvos
uint32_t updateTime = 0; //Hora da próxima atualização

int old_analog =  -999; //Valor exibido pela última vez

int value[6] = {0, 0, 0, 0, 0, 0};
int old_value[6] = { -1, -1, -1, -1, -1, -1};
int d = 0;

void setup(void) {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_WHITE);

  analogMeter(); //Desenhe medidor analógico

  updateTime = millis(); //Próxima atualização
}


void loop() {
  if (updateTime <= millis()) {
    updateTime = millis() + 100; //Atualizar medidor a cada 35 milissegundos
 
    //onda Senoidal para Teste
    //d += 4; if (d >= 360) d = 0;
    //value[0] = 50 + 50 * sin((d + 0) * 0.0174532925);

    //valor ramdomico
    //value[0] = random(0,100);

    //valor em adc0
    value [0] = analogRead(PinA0)/5;//ajuste da escala de 100% para em 75% sinalizar 300mV
    
    //unsigned long tt = millis();
    plotNeedle(value[0], 0); //Demora entre 2 e 14ms para replantar a agulha com atraso zero
    //Serial.println(millis()-tt);
  }
}

// #########################################################################
//  Desenhe o medidor analógico na tela
// #########################################################################
void analogMeter()
{

  //Contorno do medidor
  tft.fillRect(0, 0, M_SIZE*239, M_SIZE*131, TFT_GREY);
  tft.fillRect(1, M_SIZE*3, M_SIZE*234, M_SIZE*125, TFT_WHITE);

  tft.setTextColor(TFT_BLACK);  // Text colour

  //Desenhe carrapatos a cada 5 graus de -50 a +50 graus (100 graus FSD swing)
  for (int i = -50; i < 51; i += 5) {
    //Comprimento de escala longa
    int tl = 15;

    //Coordenadas de marcação para desenhar
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    uint16_t y0 = sy * (M_SIZE*100 + tl) + M_SIZE*150;
    uint16_t x1 = sx * M_SIZE*100 + M_SIZE*120;
    uint16_t y1 = sy * M_SIZE*100 + M_SIZE*150;

    //Coordenadas da próxima marcação para preenchimento de zona
    float sx2 = cos((i + 5 - 90) * 0.0174532925);
    float sy2 = sin((i + 5 - 90) * 0.0174532925);
    int x2 = sx2 * (M_SIZE*100 + tl) + M_SIZE*120;
    int y2 = sy2 * (M_SIZE*100 + tl) + M_SIZE*150;
    int x3 = sx2 * M_SIZE*100 + M_SIZE*120;
    int y3 = sy2 * M_SIZE*100 + M_SIZE*150;

    //Limites da zona amarela
    //if (i >= -50 && i < 0) {
    //  tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
    //  tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
    //}

    //Limites da zona verde
    if (i >= 0 && i < 25) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_GREEN);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_GREEN);
    }

    //Limites da zona laranja
    if (i >= 25 && i < 50) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_ORANGE);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_ORANGE);
    }

    //Comprimento da escala curta
    if (i % 25 != 0) tl = 8;

    //Recalcular as coordenadas no caso de o comprimento do carrapato ser alterado
    x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    y0 = sy * (M_SIZE*100 + tl) + M_SIZE*150;
    x1 = sx * M_SIZE*100 + M_SIZE*120;
    y1 = sy * M_SIZE*100 + M_SIZE*150;

    //Desenhar marca
    tft.drawLine(x0, y0, x1, y1, TFT_BLACK);

    //Verifique se os rótulos devem ser desenhados, com ajustes de posição
    if (i % 25 == 0) {
      // Calculate label positions
      x0 = sx * (M_SIZE*100 + tl + 10) + M_SIZE*120;
      y0 = sy * (M_SIZE*100 + tl + 10) + M_SIZE*150;
      switch (i / 25) {
        case -2: tft.drawCentreString("0", x0+4, y0-4, 1); break;
        case -1: tft.drawCentreString("25", x0+2, y0, 1); break;
        case 0: tft.drawCentreString("50", x0, y0, 1); break;
        case 1: tft.drawCentreString("75", x0, y0, 1); break;
        case 2: tft.drawCentreString("100", x0-2, y0-4, 1); break;
      }
    }

    //Agora desenhe o arco da escala
    sx = cos((i + 5 - 90) * 0.0174532925);
    sy = sin((i + 5 - 90) * 0.0174532925);
    x0 = sx * M_SIZE*100 + M_SIZE*120;
    y0 = sy * M_SIZE*100 + M_SIZE*150;
    //Desenhe um arco de escala, não desenhe a última parte
    if (i < 50) tft.drawLine(x0, y0, x1, y1, TFT_BLACK);
  }

  tft.drawString("ppm", M_SIZE*(3 + 230 - 40), M_SIZE*(119 - 20), 2); //Unidades no canto inferior direito
  tft.drawCentreString("%Gas", M_SIZE*120, M_SIZE*75, 4); //Comente para evitar a fonte 4
  tft.drawRect(1, M_SIZE*3, M_SIZE*236, M_SIZE*126, TFT_BLACK); //Desenhar linha de moldura

  plotNeedle(0, 0); //Coloque a agulha do medidor em 0
}

/*#########################################################################
Atualizar a posição da agulha
Esta função está bloqueando enquanto a agulha se move, o tempo depende de ms_delay
10 ms minimiza a cintilação da agulha se o texto for desenhado dentro da área de varredura da agulha
Valores menores OK se o texto não estiver na área de varredura, zero para movimento instantâneo, mas
não parece realista ... (nota: 100 incrementos para deflexão de escala total)
#########################################################################*/
void plotNeedle(int value, byte ms_delay)
{
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  char buf[8]; dtostrf(value, 4, 0, buf);
  tft.drawRightString(buf, 33, M_SIZE*(119 - 20), 2);

  if (value < -10) value = -10; //Limite o valor para emular os batentes da agulha
  if (value > 110) value = 110;

  //Mova a agulha até que o novo valor seja alcançado
  while (!(value == old_analog)) {
    if (old_analog < value) old_analog++;
    else old_analog--;

    if (ms_delay == 0) old_analog = value; //Atualize imediatamente se o atraso for 0

    float sdeg = map(old_analog, -10, 110, -150, -30); //Valor do mapa para ângulo
    //Calcular a ponta das coords da agulha
    float sx = cos(sdeg * 0.0174532925);
    float sy = sin(sdeg * 0.0174532925);

    //Calcule x delta do início da agulha (não começa no ponto de pivô)
    float tx = tan((sdeg + 90) * 0.0174532925);

    //Apagar imagem de agulha antiga
    tft.drawLine(M_SIZE*(120 + 24 * ltx) - 1, M_SIZE*(150 - 24), osx - 1, osy, TFT_WHITE);
    tft.drawLine(M_SIZE*(120 + 24 * ltx), M_SIZE*(150 - 24), osx, osy, TFT_WHITE);
    tft.drawLine(M_SIZE*(120 + 24 * ltx) + 1, M_SIZE*(150 - 24), osx + 1, osy, TFT_WHITE);

    //Redesenhar o texto sob a agulha
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.drawCentreString("%Gas", M_SIZE*120, M_SIZE*75, 4); //Comente para evitar a fonte 4

    //Armazene novas coords de agulha para o próximo apagamento
    ltx = tx;
    osx = M_SIZE*(sx * 98 + 120);
    osy = M_SIZE*(sy * 98 + 150);

    //Desenhe a agulha na nova posição, magenta torna a agulha um pouco mais ousada
    //Desenha 3 linhas para engrossar a agulha
    tft.drawLine(M_SIZE*(120 + 24 * ltx) - 1, M_SIZE*(150 - 24), osx - 1, osy, TFT_RED);
    tft.drawLine(M_SIZE*(120 + 24 * ltx), M_SIZE*(150 - 24), osx, osy, TFT_MAGENTA);
    tft.drawLine(M_SIZE*(120 + 24 * ltx) + 1, M_SIZE*(150 - 24), osx + 1, osy, TFT_RED);

    //Abaixe ligeiramente a agulha à medida que se aproxima da nova posição
    if (abs(old_analog - value) < 10) ms_delay += ms_delay / 5;

    //Espere antes da próxima atualização
    delay(ms_delay);
  }
}
