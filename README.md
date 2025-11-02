🏗️ Arduino 3-Axis Cardboard Crane
This is my homemade 3-axis robotic crane, built mostly from cardboard, an Arduino UNO, and a few low-cost motors.
It can rotate, lift its arm, and raise or lower a small load — just like a miniature construction crane.

⚙️ What It Does
The crane uses three different types of motors:
Stepper motor (28BYJ-48 + ULN2003) – rotates the base left and right
Servo motor (SG90) – tilts the crane arm up and down
DC motor (L293D driver + thread) – works as a winch to lift and lower items
A joystick controls the base rotation and arm movement, while two push buttons handle the winch for lifting and lowering.
The code runs all three motors at the same time without blocking, so the movement feels smooth and natural.

🧠 What I Learned
When I first powered everything on, the servo suddenly snapped to its maximum angle.
It jerked the whole crane and nearly tore the cardboard arm apart!
This happened because the servo always starts in an undefined position until it receives a command.

🧩 How I Fixed It
To fix the problem, I added a short startup motion in the setup() function:

for (int i = 150; i >= 55; i--) {
  servo.write(i);
  delay(50);
}
This simple loop moves the servo slowly to its resting position instead of jumping there instantly.
I also limited the servo angle to 0–80 degrees to keep it from straining the joints.

Other small fixes:
Used INPUT_PULLUP buttons to remove external resistors
Added a dead-zone around the joystick center to prevent jitter
Tuned PWM motor speed for better control and less vibration

⚡ Result
After a lot of testing and a few rebuilds, the crane now moves smoothly:
Joystick X → base rotation
Joystick Y → arm tilt
Buttons → lift / lower load
It’s a fun, low-cost project that taught me a lot about motor control, wiring, and debugging real hardware.
