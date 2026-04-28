  const int SWITCH_PIN = 8;

void setup() {
  Serial.begin(9600);
  pinMode(SWITCH_PIN, INPUT);
  Serial.println(F("switchTester iniciado. Pressione o interruptor para testar."));
}

void loop() {
  if (digitalRead(SWITCH_PIN) == LOW) {
    Serial.println(F("Interruptor pressionado!"));
    delay(1000); // Debounce delay
  }
}
