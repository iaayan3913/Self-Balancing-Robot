#include <Wire.h>


//  GYRO + ACCEL complementary filter 


const uint8_t MPU = 0x68; // I2C address
int16_t rawAcX, rawAcY, rawAcZ, rawGyY;
float accAngle, gyroRate, pitchAngle;
unsigned long lastTime;

// --- CALIBRATION ---
// Run the calibration script we used earlier and put your offsets here
float baseAcX = -950; 
float baseGyY = 175; 

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Wake up the MPU-6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);
  
  lastTime = micros();
}

void loop() {
  // 1. Calculate time passed (dt)
  unsigned long currentTime = micros();
  float dt = (float)(currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;

  // 2. Read Sensor Data
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start reading from Accelerometer X
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU, (size_t)14, (bool)true);

  rawAcX = Wire.read() << 8 | Wire.read();
  rawAcY = Wire.read() << 8 | Wire.read();
  rawAcZ = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); // Skip Temp
  Wire.read(); Wire.read(); // Skip Gyro X
  rawGyY = Wire.read() << 8 | Wire.read(); // We need Gyro Y for pitch

  // 3. Convert Accelerometer to Angle (Degrees)
  // We use atan2 to find the angle relative to gravity
  accAngle = atan2((float)(rawAcX + baseAcX), 16384.0) * 180 / PI;

  // 4. Convert Gyro to Degrees/Second
  gyroRate = (float)(rawGyY + baseGyY) / 131.0;

  // 5. THE COMPLEMENTARY FILTER
  // New Angle = 98% of (Old Angle + Gyro Change) + 2% of (Accel Angle)
  pitchAngle = 0.98 * (pitchAngle + gyroRate * dt) + 0.02 * (accAngle);

  // 6. Output to Serial Plotter (Ctrl+Shift+L)
  Serial.print("Accel_Angle:"); Serial.print(accAngle);
  Serial.print(",");
  Serial.print("Filtered_Angle:"); Serial.println(pitchAngle);

  delay(10); // Run at ~100Hz
}