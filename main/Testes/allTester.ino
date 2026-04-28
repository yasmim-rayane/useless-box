/*
  allTester.ino
  Teste conjunto: RGB (fade), Eyes (blink) e Sound (DFPlayer) simultaneamente.

  Pinos usados (coincidem com os testers individuais):
   - RGB: RED=5, GREEN=3, BLUE=6  (do arquivo rgbTester.ino)
   - Eyes: eye1=9, eye2=10         (do arquivo eyesTester.ino)
   - DFPlayer: ARDUINO_RX=12, ARDUINO_TX=13 (do arquivo soundTester.ino)

  Este sketch roda as três funções sem bloqueio (usa millis) exceto a inicialização do DFPlayer.
*/

#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// --- Pins ---
const int RGB_RED_PIN = 5;
const int RGB_GREEN_PIN = 3;
const int RGB_BLUE_PIN = 6;

const int EYE_PIN_1 = 9;
const int EYE_PIN_2 = 10;

#define ARDUINO_RX 13  // Arduino TX -> DFPlayer RX
#define ARDUINO_TX 12  // Arduino RX -> DFPlayer TX

// --- DFPlayer ---
SoftwareSerial mp3Serial(ARDUINO_RX, ARDUINO_TX);
DFRobotDFPlayerMini dfPlayer;
bool dfInitialized = false;

// --- RGB state ---
int redValue = 255;
int greenValue = 0;
int blueValue = 0;
int colorPhase = 0; // 0: R->G, 1: G->B, 2: B->R
const int FADE_STEP = 5;
const unsigned long RGB_INTERVAL = 70;
unsigned long rgbPrev = 0;

// --- Eyes state ---
bool eyesState = false;
const unsigned long EYES_INTERVAL = 500; // blink interval
unsigned long eyesPrev = 0;

// --- Sound state ---
int currentTrack = 1;
const int MAX_TRACK = 4; // número de faixas de teste no SD
const unsigned long SOUND_INTERVAL = 5000; // tempo entre trocas de faixa
unsigned long soundPrev = 0;

void setup() {
  Serial.begin(9600);

  // RGB pins
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  analogWrite(RGB_RED_PIN, redValue);
  analogWrite(RGB_GREEN_PIN, greenValue);
  analogWrite(RGB_BLUE_PIN, blueValue);

  // Eyes pins
  pinMode(EYE_PIN_1, OUTPUT);
  pinMode(EYE_PIN_2, OUTPUT);
  digitalWrite(EYE_PIN_1, LOW);
  digitalWrite(EYE_PIN_2, LOW);

  // Init DFPlayer
  mp3Serial.begin(9600);
  Serial.println(F("Inicializando DFPlayer..."));
  if (!dfPlayer.begin(mp3Serial)) {
    Serial.println(F("Falha ao iniciar DFPlayer. Verifique conexoes e SD."));
    dfInitialized = false;
  } else {
    Serial.println(F("DFPlayer iniciado."));
    dfPlayer.volume(20);
    dfInitialized = true;
    // start playing the first track
    dfPlayer.play(currentTrack);
  }

  // initialize timers
  rgbPrev = millis();
  eyesPrev = millis();
  soundPrev = millis();
}

void loop() {
  unsigned long now = millis();

  // RGB tick
  if (now - rgbPrev >= RGB_INTERVAL) {
    rgbPrev = now;
    rgbStep();
  }

  // Eyes tick (blink)
  if (now - eyesPrev >= EYES_INTERVAL) {
    eyesPrev = now;
    eyesState = !eyesState;
    digitalWrite(EYE_PIN_1, eyesState ? HIGH : LOW);
    digitalWrite(EYE_PIN_2, eyesState ? HIGH : LOW);
  }

  // Sound tick (change track periodically)
  if (now - soundPrev >= SOUND_INTERVAL) {
    soundPrev = now;
    currentTrack++;
    if (currentTrack > MAX_TRACK) currentTrack = 1;
    playTrack(currentTrack);
  }

  // Optional: poll DFPlayer for events (not required for simple play)
}

void rgbStep() {
  switch (colorPhase) {
    case 0: // Red -> Green
      redValue = max(0, redValue - FADE_STEP);
      greenValue = min(255, greenValue + FADE_STEP);
      if (redValue <= 0 && greenValue >= 255) {
        redValue = 0;
        greenValue = 255;
        colorPhase = 1;
      }
      break;

    case 1: // Green -> Blue
      greenValue = max(0, greenValue - FADE_STEP);
      blueValue = min(255, blueValue + FADE_STEP);
      if (greenValue <= 0 && blueValue >= 255) {
        greenValue = 0;
        blueValue = 255;
        colorPhase = 2;
      }
      break;

    case 2: // Blue -> Red
      blueValue = max(0, blueValue - FADE_STEP);
      redValue = min(255, redValue + FADE_STEP);
      if (blueValue <= 0 && redValue >= 255) {
        blueValue = 0;
        redValue = 255;
        colorPhase = 0;
      }
      break;
  }

  analogWrite(RGB_RED_PIN, redValue);
  analogWrite(RGB_GREEN_PIN, greenValue);
  analogWrite(RGB_BLUE_PIN, blueValue);
}

void playTrack(int track) {
  Serial.print(F("Tocando faixa: "));
  Serial.println(track);
  if (dfInitialized) {
    dfPlayer.play(track);
  } else {
    Serial.println(F("DFPlayer nao inicializado - ignorando play."));
  }
}
