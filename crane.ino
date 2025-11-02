#include <Servo.h>          // Library for controlling servo motor
#include <AccelStepper.h>   // Library for controlling stepper motor with acceleration support

// --- Object declarations ---
Servo servo;  // Servo motor for lifting the crane arm
AccelStepper stepper(AccelStepper::HALF4WIRE, 7, 5, 6, 4); 
// Stepper motor (28BYJ-48) connected to ULN2003 driver using pins 7,5,6,4

// --- Joystick and button pin definitions ---
const int ypin = A1;       // Joystick Y-axis (controls servo arm)
const int xpin = A0;       // Joystick X-axis (controls stepper rotation)
const int zeropos = 512;   // Joystick center value (~512 for analog)
const int buffer = 50;     // Dead-zone buffer to prevent small unwanted movement

// --- DC motor (winch) control pins via L293D ---
const int Speedpin = 3;    // PWM speed control pin (EN1 on L293D)
const int upbutt = 2;      // Button for raising the winch (active LOW)
const int downbutt = 8;    // Button for lowering the winch (active LOW)
const int uptrigg = 12;    // L293D IN1 pin for direction control
const int downtrigg = 13;  // L293D IN2 pin for direction control

// --- Motion variables ---
int angle = 55;                       // Initial servo position
static unsigned long servotick = 0;   // Timer variable for smooth servo updates

void setup() {
  // --- Attach and initialize motors ---
  servo.attach(10);                   // Servo control pin
  stepper.setMaxSpeed(3500);          // Max speed for stepper motor (steps per second)
  stepper.setAcceleration(2000);      // Acceleration rate for stepper

  // --- Set pin modes ---
  pinMode(ypin, INPUT);               // Joystick Y input
  pinMode(xpin, INPUT);               // Joystick X input
  pinMode(Speedpin, OUTPUT);          // PWM pin for DC motor speed
  pinMode(uptrigg, OUTPUT);           // DC motor direction pin 1
  pinMode(downtrigg, OUTPUT);         // DC motor direction pin 2
  pinMode(upbutt, INPUT_PULLUP);      // Up button (using internal pull-up)
  pinMode(downbutt, INPUT_PULLUP);    // Down button (using internal pull-up)

  // --- Smoothly move servo to starting angle (lower arm at startup) ---
  for (int i = 150; i >= 55; i--) {
    servo.write(i);
    delay(50); // Delay for smooth motion during startup
  }
}

void loop() {
  // --- DC Motor (winch) control section ---
  // Only allow winch movement if servo arm is not too low (angle >= 50)
  if (angle >= 50) {
    if (digitalRead(upbutt) == LOW) {
      // UP button pressed → Raise the load
      analogWrite(Speedpin, 150);      // Set motor speed (0–255)
      digitalWrite(uptrigg, HIGH);     // Rotate motor forward
      digitalWrite(downtrigg, LOW);
    } 
    else if (digitalRead(downbutt) == LOW) {
      // DOWN button pressed → Lower the load
      analogWrite(Speedpin, 70);       // Set slower speed for lowering
      digitalWrite(downtrigg, HIGH);   // Rotate motor in reverse
      digitalWrite(uptrigg, LOW);
    } 
    else {
      // No buttons pressed → Stop motor
      digitalWrite(downtrigg, LOW);
      digitalWrite(uptrigg, LOW);
    }
  }

  // --- Stepper control (crane rotation) ---
  int y_val = analogRead(ypin);        // Read Y-axis value from joystick
  int x_val = analogRead(xpin);        // Read X-axis value from joystick
  int speed = 0;                       // Stepper speed variable

  // Map joystick X movement to stepper speed (both directions)
  if (x_val < zeropos - buffer) {
    speed = map(x_val, zeropos - buffer, 0, 0, -1500);  // Rotate left
  } 
  else if (x_val > zeropos + buffer) {
    speed = map(x_val, zeropos + buffer, 1023, 0, 1500); // Rotate right
  } 
  else {
    speed = 0; // Within dead-zone → no rotation
  }

  // Update stepper with computed speed
  stepper.setSpeed(speed);
  stepper.runSpeed(); // Runs continuously at set speed

  // --- Servo control (arm lifting) ---
  unsigned long now = millis();  // Current time in milliseconds

  if (now - servotick >= 15) {   // Update every 15 ms for smooth servo motion
    if (y_val > zeropos + buffer) {
      // Joystick pushed forward → Raise arm
      angle++;
      if (angle >= 80) angle = 80;  // Limit max angle
    } 
    else if (y_val < zeropos - buffer) {
      // Joystick pulled backward → Lower arm
      angle--;
      if (angle <= 0) angle = 0;    // Limit minimum angle
    }
    servo.write(angle);             // Move servo to new angle
    servotick = now;                // Reset timer
  }

  delayMicroseconds(200);           // Small delay for stability (non-blocking)
}