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

TFT_eSPI ecran = TFT_eSPI();

int valeurPrec = 9999; // la valeur déjà affichée à l'écran (on commence par une valeur impossible afin de forcer la mise à jour)

void setup() {
  ecran.init();
  ecran.setRotation(1); // réglage de l'écran en mode paysage
  ecran.fillScreen(TFT_BLACK);  // on dessine un fond noir

  // on écrit "tension", en petits caractères, centré en haut
  // de l'écran (cet affichage ne changera pas):
  ecran.setTextColor(TFT_YELLOW);

  // positionx: 80, position y: 10, police de caractère #4
  ecran.drawCentreString("Tensao:", 80, 10, 4);

  ecran.setTextSize(2); // la valeur numérique sera écrite en taille 2
}

void loop() {

  int valeur;

  // mesure de la tension à l'entrée A0
  valeur = map(analogRead(A0), 0, 4095, 0, 330);

  if (valeur != valeurPrec) { // la valeur mesurée a changé

    int xpos = 10;      // position x du début du texte

    xpos = 10;
    // on écrit la nouvelle valeur en vert sur fond noir
    // puisqu'on a spécifié une couleur de fond, la nouvelle valeur
    // masquera celle qui était déjà inscrite à l'écran
    ecran.setTextColor(TFT_GREEN, TFT_BLACK);
    xpos += ecran.drawFloat(valeur / 100.0, 2, xpos, 40, 4);
    ecran.drawString(" V", xpos, 35, 4);

    // enveloppe de la jauge rectangulaire
    // positionx: 14, positiony: 95, largeur 134, hauteur 14, couleur noir
    ecran.fillRect( 14, 95, 134, 14, TFT_BLACK);
    ecran.drawRect( 14, 95, 134, 14, TFT_WHITE);
    // partie mobile de la jauge rectangulaire
    ecran.fillRect( 16, 97, map(valeur, 0, 330, 0, 130), 10, TFT_RED);

    valeurPrec = valeur;

    delay(1000);
  }

}
