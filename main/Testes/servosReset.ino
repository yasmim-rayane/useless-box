#include <Servo.h>

/* Servo */
const int SERVO_SKELETON_BODY_PIN = A0;
const int SERVO_SKELETON_HAND_PIN = A2;
const int SERVO_BOTTOM_DOOR = A1;
const int SERVO_TOP_DOOR = A3;

Servo bottomDoor;
Servo topDoor;
Servo skeletonBody;
Servo skeletonHand;

void setup(){
    // Servo's attach
    bottomDoor.attach(SERVO_BOTTOM_DOOR);
    topDoor.attach(SERVO_TOP_DOOR);
    skeletonBody.attach(SERVO_SKELETON_BODY_PIN);
    skeletonHand.attach(SERVO_SKELETON_HAND_PIN);

    // Servo's reset
    bottomDoor.write(180);
    topDoor.write(6);
    skeletonBody.write(30);
    skeletonHand.write(34);
}

void loop(){
    // Nothing to do here
}