// Using the "Dx" labels directly for the Nano ESP32
const int PIN_PWMA = D3; 
const int PIN_AIN1 = D4; 
const int PIN_AIN2 = D5; 

const int PIN_PWMB = D6; 
const int PIN_BIN1 = D7; // Note: This triggers the onboard Red LED
const int PIN_BIN2 = D8; 

const int PIN_STBY = D9; 

void setup() {
  Serial.begin(115200);
  
  // Set all motor pins as outputs
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);

  // Take the driver out of sleep mode
  digitalWrite(PIN_STBY, HIGH);
  
  Serial.println("--- Nano ESP32 Standard Pin Motor Test ---");
  Serial.println("Ensure 9.6V Battery is connected to VM pin!");
  delay(3000);
}

void loop() {
  // 1. Move Forward
  Serial.println("Action: BOTH FORWARD");
  driveMotors(true, 150); // 150 is a safe speed for 9.6V
  delay(2000);

  // 2. Stop
  Serial.println("Action: STOP");
  driveMotors(true, 0);
  delay(1000);

  // 3. Move Backward
  Serial.println("Action: BOTH BACKWARD");
  driveMotors(false, 150);
  delay(2000);

  // 4. Stop
  Serial.println("Action: STOP");
  driveMotors(true, 0);
  delay(3000);
}

void driveMotors(bool forward, int speed) {
  // Motor A - Keep as is
  digitalWrite(PIN_AIN1, forward ? HIGH : LOW);
  digitalWrite(PIN_AIN2, forward ? LOW : HIGH);
  analogWrite(PIN_PWMA, speed);

  // Motor B - FLIPPED logic to match Motor A
  digitalWrite(PIN_BIN1, forward ? LOW : HIGH); // Changed from HIGH : LOW
  digitalWrite(PIN_BIN2, forward ? HIGH : LOW); // Changed from LOW : HIGH
  analogWrite(PIN_PWMB, speed);
}