#include <Wire.h>
#include "GY521.h"

GY521 sensor(0x68);

// --- PID TUNING ---
float Kp = 6; float Ki = 0.000001; float Kd =0.03;   

// --- PINS ---
const int PIN_PWMA = D3; const int PIN_AIN1 = D4; const int PIN_AIN2 = D5;
const int PIN_PWMB = D6; const int PIN_BIN1 = D7; const int PIN_BIN2 = D8;
const int PIN_STBY = D9;
const int PIN_LED  = LED_BLUE; // Using the onboard Blue LED

// --- VARIABLES ---
float pitchAngle, gyroRate, error, integral;
float targetAngle = 0; 
unsigned long lastTime;
const unsigned long LOOP_TIME_US = 5000; 

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);
  digitalWrite(PIN_STBY, LOW); // Keep motors OFF during calibration

  if (!sensor.begin()) {
    while (1); // Halt if sensor missing
  }

  // --- CALIBRATION ROUTINE ---
  Serial.println("CALIBRATING: DO NOT MOVE ROBOT...");
  
  // Blink slowly while calibrating
  for(int i=0; i<5; i++) {
    digitalWrite(PIN_LED, LOW); // Nano ESP32 LED is Active LOW
    delay(100);
    digitalWrite(PIN_LED, HIGH);
    delay(100);
  }

  sensor.calibrate(1000); // Take 1000 samples to find "Zero"

  // Calibration Finished: Rapid Flashing
  for(int i=0; i<10; i++) {
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    delay(50);
  }
  digitalWrite(PIN_LED, HIGH); // Turn LED off (High is off)
  
  Serial.println("CALIBRATION DONE!");

  // Wake motors
  pinMode(PIN_PWMA, OUTPUT); pinMode(PIN_AIN1, OUTPUT); pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT); pinMode(PIN_BIN1, OUTPUT); pinMode(PIN_BIN2, OUTPUT);
  digitalWrite(PIN_STBY, HIGH);

  lastTime = micros();
}

void loop() {
  if (micros() - lastTime >= LOOP_TIME_US) {
    float dt = (float)(micros() - lastTime) / 1000000.0;
    lastTime = micros();

    sensor.read();
    pitchAngle = sensor.getAngleY(); 
    gyroRate = sensor.getGyroY();

    error = pitchAngle - targetAngle;
    
    float P = Kp * error;
    integral += error * dt;
    integral = constrain(integral, -10, 10); 
    float I = Ki * integral;
    float D = Kd * gyroRate; 

    float output = P + I + D;

    if (abs(pitchAngle) > 45) {
      moveMotors(0);
      digitalWrite(PIN_LED, LOW); // Light up LED if the robot has "failed"
    } else {
      moveMotors(output);
      digitalWrite(PIN_LED, HIGH);
    }

    Serial.print("Target:"); Serial.print(targetAngle);
    Serial.print(",");
    Serial.print("Actual:"); Serial.println(pitchAngle);
  }
}

void moveMotors(float power) {
  int deadzone = 25;
  int pwm = 0;
  if (power > 0) pwm = map(power, 0, 500, deadzone, 255);
  else if (power < 0) pwm = map(power, 0, -500, -deadzone, -255);
  
  bool dir = (pwm > 0);
  int finalPWM = constrain(abs(pwm), 0, 255);

  digitalWrite(PIN_AIN1, dir); digitalWrite(PIN_AIN2, !dir);
  analogWrite(PIN_PWMA, finalPWM);
  digitalWrite(PIN_BIN1, dir); digitalWrite(PIN_BIN2, !dir);
  analogWrite(PIN_PWMB, finalPWM);
}