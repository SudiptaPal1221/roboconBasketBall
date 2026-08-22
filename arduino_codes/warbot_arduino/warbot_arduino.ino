// Motor control pins for movement
#define M1_PWM 5   // Back-Right (BR) PWM
#define M1_DIR 6   // Back-Right (BR) Direction
#define M2_PWM 9   // Front-Right (FR) PWM
#define M2_DIR 10  // Front-Right (FR) Direction
#define M3_PWM 3   // Front-Left (FL) PWM
#define M3_DIR 4   // Front-Left (FL) Direction
#define M4_PWM 11  // Back-Left (BL) PWM
#define M4_DIR 12  // Back-Left (BL) Direction

// Dribbling mechanism pins (one IBT-2 channel)
#define DRIBBLE_RPWM 44 // PWM pin for forward rotation
#define DRIBBLE_LPWM 45 // PWM pin for reverse rotation
#define DRIBBLE_EN 40   // Enable pin (R_EN)
#define DRIBBLE_EN1 41  // Enable pin (L_EN)

// Shooting mechanism pins for two motors (two IBT-2 drivers)
#define SHOOT1_RPWM 46  // Motor 1 RPWM (First IBT-2)
#define SHOOT1_LPWM 7   // Motor 1 LPWM
#define SHOOT1_EN_R 42  // Motor 1 R_EN
#define SHOOT1_EN_L 43  // Motor 1 L_EN
#define SHOOT2_RPWM 8   // Motor 2 RPWM (Second IBT-2)
#define SHOOT2_LPWM 2   // Motor 2 LPWM
#define SHOOT2_EN_R 38  // Motor 2 R_EN
#define SHOOT2_EN_L 39  // Motor 2 L_EN

int led = 0;
char lastCommand = 'S';    // Track the last movement command
char dribblingState = 'E'; // 'U' for forward, 'D' for reverse sequence, 'E' for off
bool shootingOn = false;   // Shooting mechanism state
unsigned long dStartTime = 0; // Timestamp for 'D' sequence
bool dSequenceActive = false; // Flag for 'D' sequence
int dSequencePhase = 0;    // 0: inactive, 1: 50ms pause, 2: 4s reverse

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  // Set movement pins
  pinMode(M1_PWM, OUTPUT); pinMode(M1_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT); pinMode(M2_DIR, OUTPUT);
  pinMode(M3_PWM, OUTPUT); pinMode(M3_DIR, OUTPUT);
  pinMode(M4_PWM, OUTPUT); pinMode(M4_DIR, OUTPUT);
  pinMode(13, OUTPUT);

  // Set dribbling pins
  pinMode(DRIBBLE_RPWM, OUTPUT);
  pinMode(DRIBBLE_LPWM, OUTPUT);
  pinMode(DRIBBLE_EN, OUTPUT);
  pinMode(DRIBBLE_EN1, OUTPUT);
  digitalWrite(DRIBBLE_EN, HIGH);
  digitalWrite(DRIBBLE_EN1, HIGH);

  // Set shooting pins for two motors
  pinMode(SHOOT1_RPWM, OUTPUT);
  pinMode(SHOOT1_LPWM, OUTPUT);
  pinMode(SHOOT1_EN_R, OUTPUT);
  pinMode(SHOOT1_EN_L, OUTPUT);
  pinMode(SHOOT2_RPWM, OUTPUT);
  pinMode(SHOOT2_LPWM, OUTPUT);
  pinMode(SHOOT2_EN_R, OUTPUT);
  pinMode(SHOOT2_EN_L, OUTPUT);
  digitalWrite(SHOOT1_EN_R, HIGH);
  digitalWrite(SHOOT1_EN_L, HIGH);
  digitalWrite(SHOOT2_EN_R, HIGH);
  digitalWrite(SHOOT2_EN_L, HIGH);

  // Initialize all mechanisms
  moveOmni();     // Stop movement motors
  setDribbling(); // Turn off dribbling
  setShooting();  // Turn off shooting
  Serial.println("Mega started, waiting for NodeMCU commands...");
}

void loop() {
  // Handle 'D' sequence timing
  if (dSequenceActive) {
    unsigned long currentTime = millis();
    if (dSequencePhase == 1 && currentTime - dStartTime >= 50) {
      // End 50ms pause, start 4s reverse
      dSequencePhase = 2;
      dStartTime = currentTime;
      setDribbling();
    } else if (dSequencePhase == 2 && currentTime - dStartTime >= 4000) {
      // End 4s reverse, stop
      dSequenceActive = false;
      dSequencePhase = 0;
      dribblingState = 'E';
      setDribbling();
    }
  }

  // Process serial commands
  if (Serial1.available()) {
    char cmd = Serial1.read();
    Serial.print("Received command: ");
    Serial.println(cmd);

    switch (cmd) {
      // Movement commands
      case 'F':
        lastCommand = cmd;
        if (led == 0) { digitalWrite(13, HIGH); led = 1; }
        else { digitalWrite(13, LOW); led = 0; }
        Serial.println("Moving Forward");
        moveOmni();
        break;
      case 'B':
        lastCommand = cmd;
        Serial.println("Moving Backward");
        moveOmni();
        break;
      case 'R':
        lastCommand = cmd;
        Serial.println("Strafing Right");
        moveOmni();
        break;
      case 'L':
        lastCommand = cmd;
        Serial.println("Strafing Left");
        moveOmni();
        break;
      case 'S':
        lastCommand = cmd;
        Serial.println("Stopped");
        moveOmni();
        break;
      // Dribbling commands
      case 'U':
        dribblingState = 'U';
        dSequenceActive = false; // Cancel any 'D' sequence
        dSequencePhase = 0;
        setDribbling();
        break;
      case 'D':
        dribblingState = 'D';
        dSequenceActive = true;
        dSequencePhase = 1; // Start with 50ms pause
        dStartTime = millis();
        setDribbling();
        break;
      // Shooting commands
      case 'O':  // 'O' for ON
        shootingOn = true;
        setShooting();
        break;
      case 'V':  // 'V' for OFF
        shootingOn = false;
        setShooting();
        break;
      case 'Z':  // 'Z' for Rotate
        lastCommand = cmd;
        Serial.println("Rotating");
        moveOmni();
        break;
      default:
        Serial.println("Unknown command");
        while (Serial1.available()) Serial1.read(); // Flush buffer
        break;
    }
  }
}

void moveOmni() {
  int M1, M2, M3, M4; // Motor speeds

  // Set motor speeds based on the last movement command
  switch (lastCommand) {
    case 'F': // Forward: All motors forward
      M1 = 255; M2 = 255; M3 = 255; M4 = 255;
      break;
    case 'B': // Backward: All motors backward
      M1 = -255; M2 = -255; M3 = -255; M4 = -255;
      break;
    case 'L': // Left: FR and FL forward, BR and BL backward
      M1 = -255; M2 = 255; M3 = -255; M4 = 255;
      break;
    case 'R': // Right: BR and BL forward, FR and FL backward
      M1 = 255; M2 = -255; M3 = 255; M4 = -255;
      break;
    case 'Z': // Rotate
      M1 = -255; M2 = -255; M3 = 255; M4 = 255;
      break;
    case 'S': // Stop
    default:
      M1 = 0; M2 = 0; M3 = 0; M4 = 0;
      break;
  }

  // Constrain motor values
  M1 = constrain(M1, -255, 255);
  M2 = constrain(M2, -255, 255);
  M3 = constrain(M3, -255, 255);
  M4 = constrain(M4, -255, 255);

  // Apply motor control for movement
  analogWrite(M1_PWM, abs(M1)); digitalWrite(M1_DIR, M1 >= 0 ? HIGH : LOW); // BR
  analogWrite(M2_PWM, abs(M2)); digitalWrite(M2_DIR, M2 >= 0 ? HIGH : LOW); // FR
  analogWrite(M3_PWM, abs(M3)); digitalWrite(M3_DIR, M3 >= 0 ? HIGH : LOW); // FL
  analogWrite(M4_PWM, abs(M4)); digitalWrite(M4_DIR, M4 >= 0 ? HIGH : LOW); // BL

  // Debug output for movement
  Serial.print("M1 (BR): "); Serial.print(M1);
  Serial.print(" M2 (FR): "); Serial.print(M2);
  Serial.print(" M3 (FL): "); Serial.print(M3);
  Serial.print(" M4 (BL): "); Serial.println(M4);
}

void setDribbling() {
  if (dribblingState == 'U') {
    analogWrite(DRIBBLE_RPWM, 255);
    analogWrite(DRIBBLE_LPWM, 0);
    Serial.println("Dribbling forward");
  } else if (dribblingState == 'D' && dSequenceActive) {
    if (dSequencePhase == 1) {
      // 50ms pause
      analogWrite(DRIBBLE_RPWM, 0);
      analogWrite(DRIBBLE_LPWM, 0);
      Serial.println("Dribbling paused");
    } else if (dSequencePhase == 2) {
      // 4s reverse
      analogWrite(DRIBBLE_RPWM, 0);
      analogWrite(DRIBBLE_LPWM, 255);
      Serial.println("Dribbling reverse");
    }
  } else {
    analogWrite(DRIBBLE_RPWM, 0);
    analogWrite(DRIBBLE_LPWM, 0);
    Serial.println("Dribbling off");
  }
}

void setShooting() {
  if (shootingOn) {
    // Motor 1 clockwise (First IBT-2)
    analogWrite(SHOOT1_RPWM, 0);
    analogWrite(SHOOT1_LPWM, 255);
    // Motor 2 anticlockwise (Second IBT-2)
    analogWrite(SHOOT2_RPWM, 255);
    analogWrite(SHOOT2_LPWM, 0);
    Serial.println("Shooting on: Motor 1 clockwise, Motor 2 anticlockwise");
  } else {
    // Turn off both motors
    analogWrite(SHOOT1_RPWM, 0);
    analogWrite(SHOOT1_LPWM, 0);
    analogWrite(SHOOT2_RPWM, 0);
    analogWrite(SHOOT2_LPWM, 0);
    Serial.println("Shooting off");
  }
}
