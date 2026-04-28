/*
  rgbTester.ino
  Test sketch para fita RGB (3 canais PWM)
  Pinos usados:
   - RED: 9
   - GREEN: 10
   - BLUE: 11

  Este sketch faz um fade/ciclo suave entre cores usando millis() (não-blocking).
  Abra o Serial Monitor (9600) para ver o estado.

  Wiring:
   - Conecte a fita RGB ao driver ou aos pinos (use transistores/MOSFET adequados se a fita exigir corrente)
   - Se usar LEDs simples com resistores, ligue os anodos aos pinos e catodos a GND.
*/

const int RGB_RED_PIN = 5;
const int RGB_GREEN_PIN = 3;
const int RGB_BLUE_PIN = 6;

// Configuráveis
const int FADE_STEP = 5;                // incremento por passo (1..20)
const unsigned long INTERVAL_MS = 70;   // intervalo entre passos

// Estado interno para fade
static int redValue = 255;
static int greenValue = 0;
static int blueValue = 0;
static int colorPhase = 0; // 0: R->G, 1: G->B, 2: B->R
static unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Inicia com vermelho forte
  analogWrite(RGB_RED_PIN, redValue);
  analogWrite(RGB_GREEN_PIN, greenValue);
  analogWrite(RGB_BLUE_PIN, blueValue);

  Serial.println(F("rgbTester iniciado. Ciclo: R->G->B"));
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= INTERVAL_MS) {
    previousMillis = currentMillis;

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

    // Atualiza PWM
    analogWrite(RGB_RED_PIN, redValue);
    analogWrite(RGB_GREEN_PIN, greenValue);
    analogWrite(RGB_BLUE_PIN, blueValue);

    // Debug no Serial a cada passo
    Serial.print(F("R:")); Serial.print(redValue);
    Serial.print(F(" G:")); Serial.print(greenValue);
    Serial.print(F(" B:")); Serial.println(blueValue);
  }

  // Você pode adicionar aqui outras interações não-blocking para testes.
}
