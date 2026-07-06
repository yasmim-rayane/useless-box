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
    skeletonHandReset = 24,
    bottomDoorReset = 180,
    topDoorReset = 8;
int skeletonBodyAngled = 100,
    skeletonHandAngled = 130,
    bottomDoorAngled = 130,
    topDoorAngled = 70;

/* -------------------------------------------------------
   TIMING CONSTANTS — tweak these to adjust the feel
   without breaking the door-before-servo invariant.
   ------------------------------------------------------- */
const int DOOR_OPEN_SETTLE_MS  = 400;  // wait after door opens before moving servo
const int SERVO_DOWN_SETTLE_MS = 500;  // wait after servo resets before closing door
const int DOOR_CLOSE_SETTLE_MS = 300;  // wait after door closes before next action

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

/* =========================================================
   SAFE BUILDING-BLOCK FUNCTIONS
   These enforce: open door → wait → move → wait → reset → wait → close door
   ========================================================= */

// Raise skeleton body SAFELY (opens top door first, waits, then raises)
// Eyes always turn on when the skeleton rises.
void raiseBody() {
  eyesOn();
  topDoor.write(topDoorAngled);
  delay(DOOR_OPEN_SETTLE_MS);
  skeletonBody.write(skeletonBodyAngled);
}

// Raise body to an arbitrary angle (door opens first)
// Eyes always turn on when the skeleton rises.
void raiseBodyTo(int angle) {
  eyesOn();
  topDoor.write(topDoorAngled);
  delay(DOOR_OPEN_SETTLE_MS);
  skeletonBody.write(angle);
}

// Lower skeleton body SAFELY (resets body, waits, then closes top door)
// Eyes turn off when the skeleton goes back down.
void lowerBody() {
  skeletonBody.write(skeletonBodyReset);
  delay(SERVO_DOWN_SETTLE_MS);
  eyesOff();
  topDoor.write(topDoorReset);
  delay(DOOR_CLOSE_SETTLE_MS);
}

// Extend skeleton hand SAFELY (opens bottom door first, waits, then extends)
void extendHand() {
  bottomDoor.write(bottomDoorAngled);
  delay(DOOR_OPEN_SETTLE_MS);
  skeletonHand.write(skeletonHandAngled);
}

// Retract skeleton hand SAFELY (resets hand, waits, then closes bottom door)
void retractHand() {
  skeletonHand.write(skeletonHandReset);
  delay(SERVO_DOWN_SETTLE_MS);
  bottomDoor.write(bottomDoorReset);
  delay(DOOR_CLOSE_SETTLE_MS);
}

/* =========================================================
   COMPOSITE HELPERS
   ========================================================= */

// Wake skeleton: effects → open top door → raise body
void wakeSkeleton() {
    fadeRGB();
    eyesOn();
    delay(200);
    raiseBody();          // top door opens, then body raises
    delay(500);           // keep skeleton raised for 500 ms
}

// Rest skeleton: lower body safely → turn off effects
void restSkeleton() {
    lowerBody();          // body resets, waits, top door closes
    eyesOff();
    RGBoff();
}

// Turn switch off: extend hand through bottom door to flip switch, then retract
void turnSwitchOff(){
    extendHand();         // bottom door opens, then hand extends
    delay(500);           // hold hand on switch
    retractHand();        // hand resets, waits, bottom door closes
}

// --- helper to play a track safely ---
void playTrackSafe(int track) {
  if (track < 1) track = 1;
  if (track > 255) track = 255;
  Serial.print(F("playTrackSafe: "));
  Serial.println(track);
  myDFPlayer.play(track);
}

/* =========================================================
   16 VERSION IMPLEMENTATIONS
   Every version now guarantees:
     - Body servo ONLY moves while top door is open
     - Hand servo ONLY moves while bottom door is open
     - Servos reset with adequate delay BEFORE doors close
   ========================================================= */

void version1() { // wake + sound 1 + rest + switch off
  playTrackSafe(1);
  wakeSkeleton();
  delay(200);
  restSkeleton();
  turnSwitchOff();
}

void version2() { // quick hand + sound 2
  playTrackSafe(2);
  // sometimes show angry RGB and blink eyes before the hand
  if (random(0, 3) == 0) {
    angryRGB(300);
    blinkEyes(2, 80, 80);
  }
  // hand moves SAFELY inside bottom door open/close
  extendHand();
  delay(300);
  retractHand();
  // now flip the switch
  turnSwitchOff();
}

void version3() { // top door dramatic + sound 3
  playTrackSafe(3);
  // body moves SAFELY inside top door open/close
  raiseBody();
  delay(600);
  lowerBody();            // body resets, waits, THEN top door closes
  // flip the switch
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
  RGBoff();
  turnSwitchOff();
}

void version6() { // hand wave + body nod + sound 6
  playTrackSafe(6);
  // hand moves SAFELY inside bottom door open/close
  extendHand();
  delay(400);
  retractHand();
  // body moves SAFELY inside top door open/close
  raiseBody();
  delay(500);
  lowerBody();
  // flip the switch
  turnSwitchOff();
}

void version7() { // two sounds sequence + open top door with body peek
  playTrackSafe(7);
  delay(600);
  playTrackSafe(8);
  // body peeks out through top door SAFELY
  raiseBody();
  delay(700);
  lowerBody();
  // flip the switch
  turnSwitchOff();
}

void version8() { // tease bottom door + switch off
  playTrackSafe(9);
  // tease: open and close bottom door (no servo moves inside, so safe)
  bottomDoor.write(bottomDoorAngled);
  delay(500);
  bottomDoor.write(bottomDoorReset);
  delay(DOOR_CLOSE_SETTLE_MS);
  // flip the switch
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
  RGBoff();
  // body was never raised, so no need to lower; just flip the switch
  turnSwitchOff();
}

void version10() { // playful: hand multiple taps + sound 11
  playTrackSafe(11);
  // open bottom door ONCE, do all taps, then close it
  bottomDoor.write(bottomDoorAngled);
  delay(DOOR_OPEN_SETTLE_MS);
  for (int i=0;i<3;i++){
    skeletonHand.write(skeletonHandAngled);
    delay(250);
    skeletonHand.write(skeletonHandReset);
    delay(150);
  }
  delay(SERVO_DOWN_SETTLE_MS);       // ensure hand is fully down
  bottomDoor.write(bottomDoorReset);
  delay(DOOR_CLOSE_SETTLE_MS);
  // flip the switch
  turnSwitchOff();
}

void version11() { // dramatic body swing + sound 12
  playTrackSafe(12);
  // body moves SAFELY inside top door open/close
  raiseBody();
  delay(700);
  lowerBody();
  // flip the switch
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
  // hand moves SAFELY inside bottom door open/close
  extendHand();
  delay(400);
  retractHand();
  eyesOff();
  // flip the switch
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
  eyesOff();
  RGBoff();
  turnSwitchOff();
}

void version15() { // full sequence: wake, hand, sounds, rest
  int t = random(1,12);
  playTrackSafe(t);
  wakeSkeleton();                     // top door opens → body raises
  delay(300);
  // hand needs its own door — open bottom door while top is still open
  extendHand();
  delay(400);
  retractHand();                      // hand resets → bottom door closes
  delay(200);
  restSkeleton();                     // body resets → top door closes
  turnSwitchOff();
}

// version16: hesitating hand sequence
void version16() {
  // audio cue
  playTrackSafe(random(1,12));

  // --- First hesitation: body peeks out, retreats ---
  // open top door, eyes on
  eyesOn();
  topDoor.write(topDoorAngled);
  delay(DOOR_OPEN_SETTLE_MS);

  // body goes halfway, hesitates
  int bodyMid = (skeletonBodyReset + skeletonBodyAngled) / 2;
  skeletonBody.write(bodyMid);
  delay(500);

  // back off — body resets, WAIT, then close top door
  skeletonBody.write(skeletonBodyReset);
  delay(SERVO_DOWN_SETTLE_MS);
  eyesOff();
  topDoor.write(topDoorReset);
  delay(DOOR_CLOSE_SETTLE_MS);

  // brief pause before second attempt
  delay(300);

  // --- Second attempt: open top door again, peek ---
  eyesOn();
  topDoor.write(topDoorAngled);
  delay(DOOR_OPEN_SETTLE_MS);
  skeletonBody.write(bodyMid);
  delay(400);

  // body resets, WAIT, then close top door
  skeletonBody.write(skeletonBodyReset);
  delay(SERVO_DOWN_SETTLE_MS);
  eyesOff();
  topDoor.write(topDoorReset);
  delay(DOOR_CLOSE_SETTLE_MS);

  // --- Final attempt: hand comes out of bottom door to flip switch ---
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
