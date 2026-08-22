#include <AccelStepper.h>
#include <MultiStepper.h>

#define STEP_PIN1 2
#define DIR_PIN1 3
#define ENABLE_PIN1 4

#define STEP_PIN2 5
#define DIR_PIN2 6
#define ENABLE_PIN2 7

// Create stepper motor objects
AccelStepper motor1(AccelStepper::DRIVER, STEP_PIN1, DIR_PIN1);//AccelStepper Driver Type 1
AccelStepper motor2(AccelStepper::DRIVER, STEP_PIN2, DIR_PIN2);

// MultiStepper for synchronized movement
MultiStepper steppersControl; //create multistepper array instance

long targetPositions[2];  // Array to hold target positions
void setup() {
    pinMode(ENABLE_PIN1, OUTPUT);
    pinMode(ENABLE_PIN2, OUTPUT);

    digitalWrite(ENABLE_PIN1, LOW);  // Enable Motor 1
    digitalWrite(ENABLE_PIN2, LOW);  // Enable Motor 2

    // Set max speed and acceleration
    motor1.setMaxSpeed(1000);  
   // motor1.setAcceleration(3000);

    motor2.setMaxSpeed(1000);
    //motor2.setAcceleration(3000);

    // Add motors to MultiStepper for movement
    steppers.addStepper(motor1);
    steppers.addStepper(motor2);

    // Set initial position
    motor1.setCurrentPosition(0);
    motor2.setCurrentPosition(0);
}

void loop() {
    

    // Define a shooting motion: Move motors quickly for launch
    targetPositions[0] = 2000;  // Steps for motor1
    targetPositions[1] = -2000; // Steps for motor2 (opposite direction)

    steppers.moveTo(targetPositions);
    steppers.runSpeedToPosition();  // Execute synchronized movement

    delay(500);  // Hold  position

    // Reset to initial position
    targetPositions[0] = 0;
    targetPositions[1] = 0;
    
    steppers.moveTo(targetPositions);
    steppers.runSpeedToPosition();

    delay(1000);  // Wait for 1s before next shot
}
