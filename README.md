# Self-Balancing Robot: Nano ESP32 Edition

A two-wheeled inverted pendulum robot controlled by an **Arduino Nano ESP32**. This project demonstrates the implementation of a PID control loop, sensor fusion using a complementary filter, and real-time hardware actuation to maintain balance.

## 🎥 Balancing Demo
<!-- Upload your video to GitHub and replace the link below -->
<div align="center">
  <img src="Adobe%20Express%20-%20PID.gif" alt="Self-balancing robot demo" width="600" height="800">
</div>

---

## Disturbance Rejection 

<video src="https://github.com/user-attachments/assets/4afb7e08-c1f2-4065-b3b0-acab9b025b75" autoplay loop muted playsinline style="max-width: 100%;">
</video>

---

## 🛠 Hardware Components
* **Microcontroller:** Arduino Nano ESP32
* **IMU:** MPU-6050 (GY-521)
* **Motor Driver:** TB6612FNG
* **Motors:** TT motors (DC3V-6V)
* **Power:** 9.6V NiMH Battery Pack
* **Protection:** 100µF Capacitor
* **Lots** of duck tape

---

## 📂 Project Evolution (Code History)
This project was developed in four distinct phases. Each file represents a critical milestone in the robot's "brain" development:

1. **`Motor_Test.ino`**: Verifies the H-Bridge wiring and standardized `Dx` pin mapping for the Nano ESP32.
2. **`Complementary_Filter.ino`**: Implements a 98/2 ratio filter to combine accelerometer and gyroscope data into a stable pitch angle.
3. **`PID_code.ino`**: The first successful balancing attempt using raw I2C register reading and manual PID calculations.
4. **`PID_code_inc_gyroLib.ino`**: The final optimized version utilizing the **RobTillaart/GY521** library, automatic startup calibration, and non-blocking timers.

---

## 🚀 Control Logic
The robot maintains balance using a **PID (Proportional, Integral, Derivative)** controller running at **200Hz** (5ms loops):

* **Proportional (Kp):** Corrects the robot based on the current tilt error.
* **Integral (Ki):** Corrects for any permanent lean or drift over time.
* **Derivative (Kd):** Dampens the movement by reacting to the speed of the fall.

**Final Parameters:**
* `Kp`: 6.0
* `Ki`: 0.000001
* `Kd`: 0.03
* `Target Angle`: 0.0 

---

## 🔌 Pin Mapping
| Component | Nano ESP32 Pin | Function |
| :--- | :--- | :--- |
| **MPU-6050** | `A4 / A5` | I2C Data (SDA) / Clock (SCL) |
| **Motor A** | `D3, D4, D5` | PWM, AIN1, AIN2|
| **Motor B** | `D6, D7, D8` | PWM, BIN1, BIN2 |
| **Standby** | `D9` | Driver Enable (HIGH = ON) |
| **Status LED**| `LED_BLUE` | Status/Calibration indicator |

---

## ⚙️ Operation Instructions
1. **Calibration:** Place the robot on a flat surface. Upon power-up, the **Blue LED** will blink slowly while the `GY521` library takes 1000 samples to find "Zero".
2. **Engagement:** Once the LED flashes rapidly and turns off, the `STBY` pin is pulled HIGH and the motors engage.
3. **Safety Killswitch:** If the robot exceeds a **45° tilt**, the motors automatically shut down to prevent damage, and the Blue LED lights up.
4. **Physical Killswitch:** A manual switch was on the top to easily cut power from the battery 
