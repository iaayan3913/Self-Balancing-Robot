#include <Wire.h>

// --- PID TUNING CONSTANTS ---
float Kp = 20.0;  
float Ki = 0.0; 
float Kd = 0;   

// --- PINS (Nano ESP32 Standard) ---
const int PIN_PWMA = D3; const int PIN_AIN1 = D4; const int PIN_AIN2 = D5;
const int PIN_PWMB = D6; const int PIN_BIN1 = D7; const int PIN_BIN2 = D8;
const int PIN_STBY = D9;

const uint8_t MPU = 0x68;
float pitchAngle, gyroRate, error, lastError, integral;
float targetAngle = -3.5; // Update this to your robot's unique "Standing" angle
unsigned long lastTime;

void setup() {
  Serial.begin(115200); // High speed for real-time plotting
  Wire.begin();
  
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); Wire.write(0);
  Wire.endTransmission(true);

  pinMode(PIN_PWMA, OUTPUT); 
  pinMode(PIN_AIN1, OUTPUT); 
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT); 
  pinMode(PIN_BIN1, OUTPUT); 
  pinMode(PIN_BIN2, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);
  digitalWrite(PIN_STBY, HIGH);

  lastTime = micros(); //generate the first time step
}

void loop() {
  float dt = (float)(micros() - lastTime) / 1000000.0; //change in time
  lastTime = micros();

  readSensor(); 

  error = pitchAngle - targetAngle;
  
  float P = Kp * error;
  integral += error * dt;
  integral = constrain(integral, -10, 10); //prevent the integral error from climbing exponentially 
  float I = Ki * integral;
  float D = Kd * gyroRate; // gyro rate is rad/s which is the derivative of the error

  float output = P + I + D;
  moveMotors(output);

  if (abs(pitchAngle) > 45) moveMotors(0); // angle killswitch (if exceeds shut power to motors)

  // --- SERIAL PLOTTER SECTION ---
  Serial.print("Target:");  Serial.print(targetAngle);
  Serial.print(",");
  Serial.print("Actual:");  Serial.println(pitchAngle);

  delay(5);  //change this to use micros instead so no delays in code 
}

void moveMotors(float power) {
  // Deadzone compensation for 9.6V motors
  int deadzone = 25;
  int pwm = 0;
  
  if (power > 0) pwm = map(power, 0, 500, deadzone, 255);
  else if (power < 0) pwm = map(power, 0, -500, -deadzone, -255);
  
  bool dir = (pwm > 0);
  int finalPWM = constrain(abs(pwm), 0, 255);

  digitalWrite(PIN_AIN1, !dir); digitalWrite(PIN_AIN2, dir);
  analogWrite(PIN_PWMA, finalPWM);
  digitalWrite(PIN_BIN1, !dir); digitalWrite(PIN_BIN2, dir);
  analogWrite(PIN_PWMB, finalPWM);
}

void readSensor() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU, (size_t)14, (bool)true);
  int16_t rawAcX = Wire.read() << 8 | Wire.read();
  for(int i=0; i<8; i++) Wire.read();
  int16_t rawGyY = Wire.read() << 8 | Wire.read();

  float accAngle = atan2((float)(rawAcX - 950), 16384.0) * 180 / PI;
  gyroRate = (float)(rawGyY + 175) / 131.0;
  pitchAngle = 0.98 * (pitchAngle + gyroRate * 0.005) + 0.02 * accAngle;
}