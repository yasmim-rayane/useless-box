#include <Servo.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

/* --- Functions prototype --- */
void turnSwitchOff();
void eyesOn();
void eyesOff();
void fadeRGB();
void RGBoff();
void runVersion(int idx);
void playTrackSafe(int track);

/* --- Global variables  --- */
/* Servo */
const int SERVO_SKELETON_BODY_PIN = A0;
Servo skeletonBody;
const int SERVO_SKELETON_HAND_PIN = A1;
Servo skeletonHand;
const int SERVO_BOTTOM_DOOR = A2;
Servo bottomDoor;
const int SERVO_TOP_DOOR = A3;
Servo topDoor;
int skeletonBodyReset = 30,
    skeletonHandReset = 34,
    bottomDoorReset = 180,
    topDoorReset = 6;
int skeletonBodyAngled = 150,
    skeletonHandAngled = 130,
    bottomDoorAngled = 130,
    topDoorAngled = 70;

/* Switch */
const int SWITCH_PIN = 8;

/* RGB */
const int RGB_RED_PIN = 5;
const int RGB_GREEN_PIN = 3;
const int RGB_BLUE_PIN = 6;

/* Eyes */
const int EYE_LEFT_PIN = 9,
    EYE_RIGHT_PIN = 10;

/* Sound DFPlayer */
const int ARDUINO_RX = 13;  // Arduino TX -> DFPlayer RX
const int ARDUINO_TX = 12;  // Arduino RX -> DFPlayer TX
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setup(){
    // Servo's attach
    skeletonBody.attach(SERVO_SKELETON_BODY_PIN);
    skeletonHand.attach(SERVO_SKELETON_HAND_PIN);
    bottomDoor.attach(SERVO_BOTTOM_DOOR);
    topDoor.attach(SERVO_TOP_DOOR);

    // Servo's reset
    skeletonBody.write(skeletonBodyReset);
    skeletonHand.write(skeletonHandReset);
    bottomDoor.write(bottomDoorReset);
    topDoor.write(topDoorReset);

    // Switch
    pinMode(SWITCH_PIN, INPUT);

  // Ensure eyes and RGB pins are outputs (do not change pin assignments)
  pinMode(EYE_LEFT_PIN, OUTPUT);
  pinMode(EYE_RIGHT_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // initialize Serial and DFPlayer for audio (uses existing serial pins)
  Serial.begin(9600);
  mySerial.begin(9600);
  if (!myDFPlayer.begin(mySerial)) {
    Serial.println(F("Aviso: DFPlayer nao inicializado. Audio sera ignorado."));
  } else {
    myDFPlayer.volume(30);
  }

  // seed random
  randomSeed(analogRead(A5));
}

void loop(){
  static int lastState = LOW;
  int state = digitalRead(SWITCH_PIN);
  // detect press (LOW -> HIGH)
  if (lastState == LOW && state == HIGH) {
    // choose random version 1..16 (new hesitant-hand version added)
    int v = random(1,17);
    // pick a random RGB color each activation
    uint8_t rr = random(0,256);
    uint8_t gg = random(0,256);
    uint8_t bb = random(0,256);
    setRGBColor(rr, gg, bb);
    runVersion(v);
    // wait until switch released to avoid retrigger
    while (digitalRead(SWITCH_PIN) == HIGH) delay(10);
  }
  lastState = state;
}

void wakeSkeleton() {
    fadeRGB();
    eyesOn();
    delay(200);
    topDoor.write(topDoorAngled);
    delay(750);
  skeletonBody.write(skeletonBodyAngled);
  // keep skeleton raised for 500 ms
  delay(500);
}

void restSkeleton() {
    skeletonBody.write(skeletonBodyReset);
    delay(750);
    topDoor.write(topDoorReset);
    delay(200);
    eyesOff();
    RGBoff();
    turnSwitchOff();
}

void turnSwitchOff(){
    bottomDoor.write(bottomDoorAngled);
        delay(750);
        skeletonHand.write(skeletonHandAngled);
        delay(500);
        skeletonHand.write(skeletonHandReset);
        delay(750);
        bottomDoor.write(bottomDoorReset);
  // ensure top door (the body lid) is closed after the hand finishes
  topDoor.write(topDoorReset);
}

// --- helper to play a track safely ---
void playTrackSafe(int track) {
  if (track < 1) track = 1;
  if (track > 255) track = 255;
  Serial.print(F("playTrackSafe: "));
  Serial.println(track);
  myDFPlayer.play(track);
}

// --- 15 version implementations ---
void version1() { // wake + sound 1 + rest
  playTrackSafe(1);
  wakeSkeleton();
  delay(200);
  restSkeleton();
}

void version2() { // quick hand + sound 2
  playTrackSafe(2);
  // sometimes show angry RGB and blink eyes before the hand
  if (random(0, 3) == 0) {
    angryRGB(300);
    blinkEyes(2, 80, 80);
  }
  skeletonHand.write(skeletonHandAngled);
  delay(600);
  skeletonHand.write(skeletonHandReset);
  turnSwitchOff();
}

void version3() { // top door dramatic + sound 3
  playTrackSafe(3);
  topDoor.write(topDoorAngled);
  delay(800);
  skeletonBody.write(skeletonBodyAngled);
  delay(600);
  skeletonBody.write(skeletonBodyReset);
  topDoor.write(topDoorReset);
  turnSwitchOff();
}

void version4() { // blink eyes and short sound
  playTrackSafe(4);
  for (int i=0;i<3;i++){
    eyesOn(); delay(200); eyesOff(); delay(200);
  }
  turnSwitchOff();
}

void version5() { // rgb party + sound 5
  playTrackSafe(5);
  for (int i=0;i<6;i++){
    // occasional angry flash
    if (random(0,6) == 0) {
      angryRGB(200);
    }
    fadeRGB();
    delay(150);
  }
  turnSwitchOff();
}

void version6() { // hand wave + body nod + sound 6
  playTrackSafe(6);
  skeletonHand.write(skeletonHandAngled);
  delay(400);
  skeletonHand.write(skeletonHandReset);
  skeletonBody.write(skeletonBodyAngled);
  delay(500);
  skeletonBody.write(skeletonBodyReset);
  turnSwitchOff();
}

void version7() { // two sounds sequence + open top
  playTrackSafe(7);
  delay(600);
  playTrackSafe(8);
  topDoor.write(topDoorAngled);
  delay(700);
  topDoor.write(topDoorReset);
  turnSwitchOff();
}

void version8() { // reverse: close bottom then hand
  playTrackSafe(9);
  bottomDoor.write(bottomDoorAngled);
  delay(500);
  bottomDoor.write(bottomDoorReset);
  turnSwitchOff();
}

void version9() { // long wake with eyes + rgb + sound 10
  playTrackSafe(10);
  // blink while waking and keep eyes on
  eyesOn();
  blinkEyes(3, 150, 120);
  fadeRGB();
  delay(1200);
  eyesOff();
  restSkeleton();
}

void version10() { // playful: hand multiple taps + sound 11
  playTrackSafe(11);
  for (int i=0;i<3;i++){
    skeletonHand.write(skeletonHandAngled);
    delay(250);
    skeletonHand.write(skeletonHandReset);
    delay(150);
  }
  turnSwitchOff();
}

void version11() { // dramatic body swing + sound 12
  playTrackSafe(12);
  skeletonBody.write(skeletonBodyAngled);
  delay(700);
  skeletonBody.write(skeletonBodyReset);
  turnSwitchOff();
}

void version12() { // combine sounds 1..3 quickly
  for (int t=1;t<=3;t++){
    playTrackSafe(t);
    delay(400);
  }
  turnSwitchOff();
}

void version13() { // eyes on long + subtle hand
  playTrackSafe(random(1,12));
  // expressive: blink and sometimes angry red
  blinkEyes(1, 300, 100);
  if (random(0,4) == 0) angryRGB(350);
  eyesOn();
  delay(900);
  skeletonHand.write(skeletonHandAngled);
  delay(400);
  skeletonHand.write(skeletonHandReset);
  eyesOff();
  turnSwitchOff();
}

void version14() { // rgb fast strobe + sound random
  playTrackSafe(random(1,12));
  for (int i=0;i<8;i++){
    analogWrite(RGB_RED_PIN, random(0,256));
    analogWrite(RGB_GREEN_PIN, random(0,256));
    analogWrite(RGB_BLUE_PIN, random(0,256));
    // eyes blink in sync sometimes
    if (i % 2 == 0) {
      eyesOn();
    } else {
      eyesOff();
    }
    delay(80);
  }
  RGBoff();
  turnSwitchOff();
}

void version15() { // full sequence: wake, hand, sounds, rest
  int t = random(1,12);
  playTrackSafe(t);
  wakeSkeleton();
  delay(300);
  skeletonHand.write(skeletonHandAngled);
  delay(400);
  skeletonHand.write(skeletonHandReset);
  delay(200);
  restSkeleton();
}

// version16: hesitating hand sequence
void version16() {
  // audio cue
  playTrackSafe(random(1,12));

  // open top door slightly
  topDoor.write(topDoorAngled);
  delay(400);

  // body goes halfway, hesitates, then returns
  int bodyMid = (skeletonBodyReset + skeletonBodyAngled) / 2;
  skeletonBody.write(bodyMid);
  delay(500);
  // back off a bit
  skeletonBody.write(skeletonBodyReset);
  delay(400);

  // lower the top door briefly
  topDoor.write(topDoorReset);
  delay(300);

  // raise top door again and final approach
  topDoor.write(topDoorAngled);
  delay(600);

  // final attempt to turn switch off
  skeletonHand.write(skeletonHandAngled);
  delay(450);
  skeletonHand.write(skeletonHandReset);
  delay(200);

  // complete the turn-off sequence (bottom door + hand) to ensure switch is off
  turnSwitchOff();
}

// runner
void runVersion(int idx) {
  switch (idx) {
    case 1: version1(); break;
    case 2: version2(); break;
    case 3: version3(); break;
    case 4: version4(); break;
    case 5: version5(); break;
    case 6: version6(); break;
    case 7: version7(); break;
    case 8: version8(); break;
    case 9: version9(); break;
    case 10: version10(); break;
    case 11: version11(); break;
    case 12: version12(); break;
    case 13: version13(); break;
    case 14: version14(); break;
    case 15: version15(); break;
    case 16: version16(); break;
    default: version1(); break;
  }
}

void eyesOn() {
  digitalWrite(EYE_LEFT_PIN, HIGH);
  digitalWrite(EYE_RIGHT_PIN, HIGH);
}

void eyesOff() {
  digitalWrite(EYE_LEFT_PIN, LOW);
  digitalWrite(EYE_RIGHT_PIN, LOW);
}

void fadeRGB() {
  const unsigned long INTERVAL_MS = 40; // tempo entre passos (ajuste para velocidade)
  const int STEP = 4; // quanto cada canal muda por passo (ajuste para suavidade)

  static unsigned long previousMillis = 0;
  static int red = 255;
  static int green = 0;
  static int blue = 0;
  static int phase = 0; // 0: R->G, 1: G->B, 2: B->R

  unsigned long now = millis();
  if (now - previousMillis < INTERVAL_MS) return;
  previousMillis = now;

  switch (phase) {
    case 0: // Red -> Green
      red = max(0, red - STEP);
      green = min(255, green + STEP);
      if (red <= 0) {
        red = 0; green = 255; phase = 1;
      }
      break;

    case 1: // Green -> Blue
      green = max(0, green - STEP);
      blue = min(255, blue + STEP);
      if (green <= 0) {
        green = 0; blue = 255; phase = 2;
      }
      break;

    case 2: // Blue -> Red
      blue = max(0, blue - STEP);
      red = min(255, red + STEP);
      if (blue <= 0) {
        blue = 0; red = 255; phase = 0;
      }
      break;
  }

  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);
}

void RGBoff() {
  analogWrite(RGB_RED_PIN, 0);
  analogWrite(RGB_GREEN_PIN, 0);
  analogWrite(RGB_BLUE_PIN, 0);
}

// --- expressive helpers (do not change pins or constants) ---
// set RGB to a specific color
void setRGBColor(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(RGB_RED_PIN, r);
  analogWrite(RGB_GREEN_PIN, g);
  analogWrite(RGB_BLUE_PIN, b);
}

// angry red for a short duration (non-destructive, will simply set red then restore off)
void angryRGB(unsigned long durationMs) {
  // quick pulse in red
  setRGBColor(255, 0, 0);
  delay(durationMs);
  // restore to off; fadeRGB may resume later when called
  RGBoff();
}

// blink eyes n times
void blinkEyes(int times, unsigned long onMs, unsigned long offMs) {
  for (int i = 0; i < times; i++) {
    eyesOn();
    delay(onMs);
    eyesOff();
    delay(offMs);
  }
}
