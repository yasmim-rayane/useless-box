90nclude <Servo.h>

/* Servo */
const int SERVO_LEFT_PIN = A0;

Servo leftDoor;

void setup(){
    // Servo's attach
    leftDoor.attach(SERVO_LEFT_PIN);

    // Servo's reset
    leftDoor.write(90);
}

void loop(){
    // Nothing to do here
}