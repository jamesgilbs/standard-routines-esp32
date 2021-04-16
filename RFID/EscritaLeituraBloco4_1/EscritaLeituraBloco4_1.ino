// ------ Bibliotecas --------
#include <SPI.h>
#include <MFRC522.h> 
//-------- SPI configuração do pinos do Leitor-----------
#define SDA 21 //Conectado ao pino D5 do ESP32
#define RST 22 //Conectado ao pino VP do ESP32
MFRC522 mfrc522(SDA, RST);
//-------- LED RGB-----------
int RED = 32;
int BLUE = 12;
int GREEN = 31;

void setup()
{
  Serial.begin(115200);
  pinMode(RED, OUTPUT);      //RED D15 do ESP32
  pinMode(GREEN, OUTPUT);    //GREEN D4 do ESP32
  pinMode(BLUE, OUTPUT);     //BLUE D2 do ESP32
  SPI.begin();               // Inicia o protocolo SPI
  mfrc522.PCD_Init();        // Inicia o Leitor
  Serial.println("Aproxime o cartão/chaveiro e Digite: 1 - Leitura ou 2 - Armazenamento");

}
void loop()
{
  digitalWrite(RED, LOW);   //Desliga LED Red
  digitalWrite(GREEN, LOW); //Desliga LED Green
  digitalWrite(BLUE, HIGH); //Liga LED Blue

  //Armazena os caracteres escritos no Monitor Serial
  char choice = Serial.read();

  if (choice == '1')
  { // Se receber 1
    Serial.println("Opção 1: Leitura de informações");
    leitor(); //Função Leitor

  }

  else if (choice == '2')
  { // Se receber 2
    Serial.println("Opção 2: Armazenamento de informações");
    escritor(); //Função Gravador
  }


}
void leitor()
{
  digitalWrite(BLUE, LOW);   //Desliga LED Blue
  digitalWrite(GREEN, HIGH); //Liga LED Green

  // Prepara chave de segurança no formato FFFFFFFFFFFFh
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Procura por novos cartões
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Reenicia a leitura
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  //Variável para controlar os blocos de memórias
  byte block;
  //Variável para acessar as linhas
  byte len;
  //Verifica o status de leitura
  MFRC522::StatusCode status;
  //Quando encontrar alguma informação escreve no monitor serial
  Serial.println(F("----Informações Encontradas----"));
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

  //---Variável 1: Nome ---

  byte buffer1[18];
  //Direciona a leitura ao bloco 4
  block = 4;
  len = 18;
  Serial.print(F("Nome: "));
  // Coleta as informações armazenadas no bloco 4
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
  status = mfrc522.MIFARE_Read(block, buffer1, &len);

  //Escreve as informações no monitor serial
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      Serial.write(buffer1[i]);
    }
  }
  Serial.print(" ");

  //---Variável 2: RG ---

  byte buffer2[18];
  //Direciona a leitura ao bloco 1
  block = 1;
  // Coleta as informações armazenadas no bloco 1
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  status = mfrc522.MIFARE_Read(block, buffer2, &len);

  Serial.print(F("RG: "));
  //Escreve as informações no monitor serial
  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(buffer2[i] );
  }
  Serial.println(F("\n----Fim----\n"));

  delay(1000);
  // Se tiver finalizado a leitura
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  setup();  // Retorna ao void Setup
}

void escritor()
{
  digitalWrite(BLUE, LOW); //Desliga LED Blue
  digitalWrite(RED, HIGH); //Liga LED Red
  // Prepara chave de segurança no formato FFFFFFFFFFFFh
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Procura por novos cartões
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Reenicia a leitura
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.print(F("ID:"));    // Indica o valor de ID (UID)
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print(F(" Tipo: "));   // Tipo
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte buffer[34];
  byte block;
  MFRC522::StatusCode status;
  byte len;
  // Aguarda até 20 segundo pelas informações
  Serial.setTimeout(20000L) ;
  // Variável 1: RG, finalize o Valor desejado com #
  Serial.println(F("Escreva o RG, finalizando com #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 30) ; // Le o nome para a serial
  for (byte i = len; i < 30; i++) buffer[i] = ' ';

  //Direciona as informações ao bloco 1
  block = 1;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));

  // Escreve no bloco 1
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  Serial.println(F("Valores armazenados no bloco 1"));


  // Variável 2: Nome, finalize o nome desejado com #
  Serial.println(F("Escreva o nome, finalizando com #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 20) ;
  for (byte i = len; i < 20; i++) buffer[i] = ' ';

  //Direciona as informações ao bloco 4
  block = 4;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));

  // Escreve no bloco 4
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  Serial.println(F("Valores armazenados no bloco 4"));
  Serial.println(F(" "));

  Serial.println(F("Informações Armazenadas com Sucesso."));


  Serial.println(" ");
  // Ao finalizar 
  mfrc522.PICC_HaltA(); 
  mfrc522.PCD_StopCrypto1(); 
  setup();// Retorna ao void Setup
}
