#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Define os pinos de comunicação
#define ARDUINO_RX 13  // Arduino TX -> DFPlayer RX
#define ARDUINO_TX 12  // Arduino RX -> DFPlayer TX

SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  Serial.println(F("Iniciando DFPlayer..."));

  if (!myDFPlayer.begin(mySerial)) {  
    Serial.println(F("Falha ao iniciar DFPlayer Mini:"));
    Serial.println(F("1. Verifique conexões"));
    Serial.println(F("2. Verifique o cartão SD"));
    while(true);
  }

  Serial.println(F("DFPlayer Mini iniciado com sucesso!"));
  myDFPlayer.volume(20);  // Volume de 0 a 30
  myDFPlayer.play(1);     // Toca o primeiro arquivo do SD
}

void loop() {
  // Você pode adicionar comandos extras aqui
  // Exemplo: mudar de faixa, pausar, etc.
}
