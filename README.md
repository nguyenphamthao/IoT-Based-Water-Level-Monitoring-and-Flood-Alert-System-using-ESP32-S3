# ESP32-S3 Water Level Monitoring and Flood Warning System

This project implements an IoT-based **water level monitoring and flood warning system** using an **ESP32-S3**, an **HC-SR04 ultrasonic sensor**, a **rain/water sensor**, an **OLED display**, LED indicators, a buzzer, and a web dashboard.

The system measures the distance between the ultrasonic sensor and the water surface, calculates the current water level, detects rain status, and classifies the situation into different warning levels such as **SAFE**, **CAUTION**, and **ALARM**. The measured data is displayed locally on the OLED screen and remotely on a web interface through WiFi.

---

## 1. Project Overview

Flooding and rising water levels can cause damage to houses, roads, drainage systems, and low-lying areas. This project aims to simulate a simple automatic monitoring station that can observe water level conditions and provide warning signals in real time.

The system is designed to:

* Measure the distance from the sensor to the water surface.
* Calculate the current water level.
* Detect whether rain is present.
* Display real-time data on an OLED screen.
* Send monitoring data to a web dashboard through WiFi.
* Activate visual and audio warning outputs based on the water level.

---

## 2. System Features

* Real-time water level measurement using ultrasonic sensing.
* Rain detection using a water/rain sensor.
* OLED display for local monitoring.
* Web dashboard for remote observation.
* LED indicators for different warning states.
* Buzzer warning for dangerous water levels.
* WiFi communication using ESP32-S3.
* Simulation and testing using Cirkit Designer.

---

## 3. Hardware Components

| Component                 | Function                                                     |
| ------------------------- | ------------------------------------------------------------ |
| ESP32-S3                  | Main microcontroller, WiFi communication, data processing    |
| HC-SR04 Ultrasonic Sensor | Measures distance from the sensor to the water surface       |
| Rain / Water Sensor       | Detects rain or water presence                               |
| OLED Display              | Shows rain status, distance, water level, and warning status |
| Red LED                   | Indicates dangerous or alarm condition                       |
| Yellow LED                | Indicates caution or warning condition                       |
| Green LED                 | Indicates safe condition                                     |
| Buzzer                    | Provides audio warning when water level becomes dangerous    |
| Jumper Wires              | Connects all components in the circuit                       |
| Web Interface             | Displays monitoring data remotely                            |

---

## 4. System Architecture

The system consists of four main layers:

1. **Sensor Layer**

   * HC-SR04 ultrasonic sensor measures the distance to the water surface.
   * Rain sensor detects whether rain is present.

2. **Controller Layer**

   * ESP32-S3 reads sensor data.
   * Calculates the current water level.
   * Determines the warning status.
   * Controls LEDs, buzzer, OLED display, and WiFi transmission.

3. **Display and Warning Layer**

   * OLED shows local monitoring information.
   * LEDs indicate safe, caution, or alarm states.
   * Buzzer alerts users when the water level becomes dangerous.

4. **Web Monitoring Layer**

   * ESP32-S3 sends data to the web server.
   * The web dashboard displays weather condition, surface distance, current water level, and warning status.

---

## 5. Working Principle

The ultrasonic sensor measures the distance between the sensor and the water surface. When the water level rises, the distance becomes smaller. Based on the measured distance, the system calculates the current water level.

The system also reads the rain sensor to determine whether it is raining. After processing both water level and rain status, ESP32-S3 classifies the condition into one of the following states:

| Status  | Meaning                                           |
| ------- | ------------------------------------------------- |
| SAFE    | Water level is low and there is no immediate risk |
| CAUTION | Water level is rising and should be monitored     |
| ALARM   | Water level is high and may indicate flood risk   |

---

## 6. OLED Display Information

The OLED screen displays:

```text
Mua: CO MUA / KHONG MUA
Khoang cach: XX.X cm
Muc nuoc: XX.X cm
AN TOAN / CHU Y / BAO DONG
```

Meaning:

* **Mua**: Rain status
* **Khoang cach**: Distance from sensor to water surface
* **Muc nuoc**: Calculated water level
* **AN TOAN**: Safe condition
* **CHU Y**: Caution condition
* **BAO DONG**: Alarm condition

---

## 7. Web Dashboard

The web dashboard displays real-time monitoring information in a simple and clear interface.

Displayed values include:

* Weather condition
* Distance to water surface
* Current water level
* Warning status

Example dashboard fields:

```text
Weather: CO MUA / KHONG MUA
Surface Distance: XX.X cm
Current Water Level: XX.X cm
Status: AN TOAN / CHU Y / BAO DONG
```

---

## 8. Warning Logic

The warning logic is based on the calculated water level.

General behavior:

* When the water level is low, the system displays **SAFE**.
* When the water level increases to a moderate range, the system displays **CAUTION**.
* When the water level exceeds the danger threshold, the system displays **ALARM** and activates warning outputs.

Example conditions:

| Water Level Condition | System Status | Output           |
| --------------------- | ------------- | ---------------- |
| Low water level       | SAFE          | Green LED        |
| Medium water level    | CAUTION       | Yellow LED       |
| High water level      | ALARM         | Red LED + Buzzer |

---

## 9. Test Cases

### Case 1: No Rain – Caution Water Level

The system detects no rain, but the calculated water level is about **50.9 cm**. Therefore, the system displays **CAUTION**, indicating that the water level should be monitored even though there is no rainfall.

### Case 2: No Rain – Safe Condition

The system detects no rain and the water level is **0 cm**. The distance from the sensor to the water surface is large, so the system correctly identifies the condition as **SAFE**.

### Case 3: Rain Detected – Caution Condition

Rain is detected and the calculated water level is about **77.4 cm**. The system switches to **CAUTION** mode, showing that the water level is rising and may become dangerous if it continues to increase.

### Case 4: Rain Detected – Alarm Condition

Rain is detected and the water level reaches about **100.4 cm**. This exceeds the danger threshold, so the system activates the **ALARM** status to indicate a potential flood risk.

---

## 10. Software Structure

The project includes two main software parts:

1. **ESP32-S3 Firmware**

   * Reads ultrasonic sensor data.
   * Reads rain sensor state.
   * Calculates water level.
   * Controls LEDs and buzzer.
   * Displays data on OLED.
   * Sends data through WiFi.

2. **Web Server**

   * Receives water level data from ESP32-S3.
   * Displays monitoring information on a web dashboard.
   * Provides a simple user interface for remote observation.

---

## 11. Technologies Used

* ESP32-S3
* HC-SR04 Ultrasonic Sensor
* Rain / Water Sensor
* OLED Display
* LED Indicators
* Buzzer
* C/C++ Arduino Framework
* WiFi Communication
* Web Dashboard
* Python Flask
* HTML / CSS
* Cirkit Designer

---

## 12. Repository Structure

```text
.
├── firmware/
│   ├── sketch.ino
│   └── documentation.txt
│
├── web/
│   ├── server.py
│   ├── requirements.txt
│   └── public/
│
├── docs/
│   └── images/
│       ├── circuit_simulation.png
│       ├── web_dashboard_safe.png
│       ├── web_dashboard_caution.png
│       └── web_dashboard_alarm.png
│
├── README.md
└── .gitignore
```

---

## 13. How to Run

### 1. Run the ESP32-S3 simulation

Open the circuit project in **Cirkit Designer** and start the simulation.

Adjust the ultrasonic sensor distance and rain sensor state to test different warning conditions.

### 2. Run the web server

Install required Python packages:

```bash
pip install -r requirements.txt
```

Run the Flask server:

```bash
python server.py
```

Open the web dashboard in the browser:

```text
http://localhost:5000
```

---

## 14. Results

The system successfully demonstrates the operation of an automatic water level monitoring station. It can measure water level, detect rain, display local information on OLED, update a web dashboard, and activate warning outputs depending on the risk level.

The simulation results confirm that the system responds correctly in safe, caution, and alarm conditions.

---

## 15. Future Development

Possible improvements include:

* Add real-time data logging.
* Add charts to visualize water level changes over time.
* Add MQTT or cloud database integration.
* Add mobile app notification.
* Improve sensor calibration for higher accuracy.
* Add solar power or battery backup.
* Design a PCB for real hardware deployment.
* Add waterproof enclosure for outdoor use.

---

## 16. Author

**Pham Thao Nguyen**
Electronics and Telecommunications Engineering
Ho Chi Minh City University of Technology – VNU-HCM
