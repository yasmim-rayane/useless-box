const int EYE_LEFT_PIN = 9, EYE_RIGHT_PIN = 10;

void eyesOn();
void eyesOff();

void setup() {
  Serial.begin(9600);
  pinMode(EYE_LEFT_PIN, OUTPUT);
  pinMode(EYE_RIGHT_PIN, OUTPUT);

  Serial.println(F("eyesTester iniciado. Piscar a cada segundo."));
}

void loop() {
  eyesOn();
  Serial.println(F("Olhos ON"));
  delay(1000);
  eyesOff();
  Serial.println(F("Olhos OFF"));
  delay(1000);
}

// Eyes control
void eyesOn() {
  // If you prefer PWM brightness, replace digitalWrite with analogWrite(…, value);
  digitalWrite(EYE_LEFT_PIN, HIGH);
  digitalWrite(EYE_RIGHT_PIN, HIGH);
}

void eyesOff() {
  digitalWrite(EYE_LEFT_PIN, LOW);
  digitalWrite(EYE_RIGHT_PIN, LOW);
}