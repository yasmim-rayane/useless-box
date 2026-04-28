#include <Servo.h>

/* Servo */
const int SERVO_LEFT_PIN = A0;
const int SERVO_RIGHT_PIN = A2;
const int SERVO_SKELETON_BODY_PIN = A1;
const int SERVO_SKELETON_HAND_PIN = A3;

Servo leftDoor;
Servo rightDoor;
Servo skeletonBody;
Servo skeletonHand;

/* Toogle Switch */
const int TOGGLE_SWITCH_PIN = 12;

void setup(){
  // Servo's attach
  leftDoor.attach(SERVO_LEFT_PIN);
  rightDoor.attach(SERVO_RIGHT_PIN);
  skeletonBody.attach(SERVO_SKELETON_BODY_PIN);
  skeletonHand.attach(SERVO_SKELETON_HAND_PIN);

  // Servo's reset
  leftDoor.write(0);
  rightDoor.write(0);
  skeletonBody.write(0);
  skeletonHand.write(0);

  // Toogle switch
  pinMode(TOGGLE_SWITCH_PIN, INPUT);
}

int main(){
    int isToogleSwitchSwitched = digitalRead(TOGGLE_SWITCH_PIN);

    if (isToogleSwitchSwitched) {
        leftDoor.write(90);
        rightDoor.write(90);
        delay(500);
        skeletonBody.write(90);
        skeletonHand.write(180);
        delay(500);
    }
}