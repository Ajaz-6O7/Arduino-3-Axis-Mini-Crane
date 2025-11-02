#include <Servo.h>
#include <AccelStepper.h>
//Test Comment 
Servo servo;
AccelStepper stepper(AccelStepper::HALF4WIRE, 7, 5, 6, 4);

const int ypin = A1;
const int xpin = A0;
const int zeropos = 512;
const int buffer = 50;
const int Speedpin = 3;
const int upbutt = 2;
const int downbutt = 8;
const int uptrigg = 12;
const int downtrigg = 13;
int angle = 55;
static unsigned long servotick = 0;

void setup() {
  servo.attach(10);
  stepper.setMaxSpeed(3500);
  stepper.setAcceleration(2000);

  pinMode(ypin, INPUT);
  pinMode(xpin, INPUT);
  pinMode(Speedpin, OUTPUT);
  pinMode(uptrigg, OUTPUT);
  pinMode(downtrigg, OUTPUT);
  pinMode(upbutt, INPUT_PULLUP);
  pinMode(downbutt, INPUT_PULLUP);

  for (int i = 150; i >= 55; i--) {
    servo.write(i);
    delay(50);
  }
}

void loop() {
  if (angle >= 50) {
    if (digitalRead(upbutt) == LOW) {
      analogWrite(Speedpin, 150);
      digitalWrite(uptrigg, HIGH);
      digitalWrite(downtrigg, LOW);
    } else if (digitalRead(downbutt) == LOW) {
      analogWrite(Speedpin, 70);
      digitalWrite(downtrigg, HIGH);
      digitalWrite(uptrigg, LOW);

    } else {
      digitalWrite(downtrigg, LOW);
      digitalWrite(uptrigg, LOW);
    }
  }

  int y_val = analogRead(ypin);
  int x_val = analogRead(xpin);
  int speed = 0;

  if (x_val < zeropos - buffer) {
    speed = map(x_val, zeropos - buffer, 0, 0, -1500);
  } else if (x_val > zeropos + buffer) {
    speed = map(x_val, zeropos + buffer, 1023, 0, 1500);
  } else {
    speed = 0;
  }
  stepper.setSpeed(speed);
  stepper.runSpeed();

  unsigned long now = millis();

  if (now - servotick >= 15) {
    if (y_val > zeropos + buffer) {
      angle++;
      if (angle >= 80) { 
        angle = 80; }
    } else if (y_val < zeropos - buffer) {
      angle--;
      if (angle <= 0) {
         angle = 0; }
    }
    servo.write(angle);
    servotick = now;
  }
  delayMicroseconds(200);
}
