# smart_cool
# 🌱 SMARTCOOL
### An IoT-Based Greenhouse Monitoring System with Edge-Based Overheating Prediction

**SmartCool — Smarter Monitoring, Better Growing.**

## 1. Project Overview

SmartCool is an IoT-based greenhouse monitoring and automation system designed to monitor environmental conditions and support better greenhouse management.

The system uses an ESP32 microcontroller to collect environmental readings from sensors, control a ventilation fan based on temperature conditions, and activate a warning LED when the configured gas-sensor threshold is reached.

Through Wi-Fi communication, the ESP32 sends sensor information to a Flask backend, which connects the hardware system to a web-based dashboard. SmartCool also incorporates a machine learning model developed to identify environmental conditions associated with potential greenhouse overheating.

The project combines embedded systems, IoT communication, web technologies and machine learning in an agricultural application.

## 2. Main Features

- **Temperature monitoring:** Measures greenhouse temperature using the SHT31 sensor.
- **Humidity monitoring:** Measures relative humidity.
- **Light monitoring:** Uses an LDR to detect changes in surrounding light conditions.
- **Gas-level monitoring:** Uses an MQ-7B sensor to obtain gas-related analogue readings.
- **Automatic ventilation:** Controls a DC fan according to configured temperature thresholds.
- **Gas warning:** Activates a warning LED when the configured gas-sensor reading threshold is reached.
- **Wi-Fi communication:** Sends sensor readings from the ESP32 to the Flask backend.
- **Web dashboard:** Provides a digital interface for monitoring greenhouse information.
- **Machine learning integration:** Incorporates an exported Decision Tree model into the embedded development environment for overheating classification.

## 3. System Architecture

SmartCool consists of the following main sections:

1. **Sensing Layer:** Collects temperature, humidity, light and gas-related readings.
2. **Processing and Control Layer:** The ESP32 processes sensor readings and controls the fan and warning LED.
3. **Communication Layer:** Wi-Fi enables communication between the ESP32 and backend.
4. **Backend Layer:** A Flask application receives sensor information.
5. **Dashboard Layer:** The web interface presents monitoring information.
6. **Machine Learning Component:** A trained Decision Tree model is exported into C code for embedded integration.

### System Data Flow

```text
       ENVIRONMENTAL SENSORS
     SHT31 | LDR | MQ-7B
              |
              v
         ESP32 BOARD
              |
       -----------------
       |               |
       v               v
  FAN CONTROL      GAS WARNING
       |               |
       -----------------
              |
              v
        Wi-Fi Communication
              |
              v
        FLASK BACKEND
              |
              v
        WEB DASHBOARD

   EMBEDDED ML MODEL INTERFACE
       Integrated with ESP32
```

## 4. Hardware Requirements

| Component | Purpose |
|---|---|
| DFRobot FireBeetle ESP32 | Main controller |
| SHT31 | Temperature and humidity sensing |
| LDR | Light detection |
| MQ-7B | Gas-related sensing |
| DFRobot 130 DC motor module V1.0 | Fan control |
| DC fan | Greenhouse ventilation |
| LED | Warning indication |
| Connecting wires | Hardware connections |
| Breadboard | Circuit assembly |

**Note:** The LCD 1602 RGB display was not included in the final implementation.

## 5. Hardware Pin Configuration

The implemented GPIO assignments are:

| Component | ESP32 Pin |
|---|---|
| SHT31 SDA | GPIO 21 |
| SHT31 SCL | GPIO 22 |
| LDR analogue output | GPIO 34 |
| MQ-7B analogue output | GPIO 35 |
| Fan control | GPIO 27 |
| Warning LED | GPIO 26 |

The SHT31 uses I²C communication and was detected at address `0x45`.

## 6. Software Requirements

The project uses the following software technologies:

- **Arduino IDE:** ESP32 firmware development and uploading.
- **C/C++:** Embedded programming and machine learning model integration.
- **Python:** Backend and machine learning development.
- **Flask:** Backend application and API.
- **HTML:** Dashboard structure.
- **CSS:** Dashboard styling.
- **JavaScript:** Dashboard functionality.
- **Scikit-learn:** Machine learning model development.

## 7. Project Structure

The main project files are organised into hardware, backend, model and dashboard components.

An example structure is:

```text
SmartCool/
│
├── ESP32/
│   └── smartcool.ino
│
├── Backend/
│   └── app.py
│
├── Model/
│   ├── smartcool_model.pkl
│   └── SmartCoolModel.h
│
├── Dashboard/
│   ├── index.html
│   ├── style.css
│   └── script.js
│
└── README.md
```

*The structure above is a guide. Adjust folder and filename entries to match the actual files in your repository.*

## 8. How the System Works

### Step 1: Environmental Data Collection

The SHT31, LDR and MQ-7B sensors collect environmental readings and send them to the ESP32.

### Step 2: Local Processing

The ESP32 processes the sensor readings and applies the configured control conditions.

### Step 3: Automatic Fan Control

The fan uses two temperature thresholds:

- At or above **30°C**, the fan switches ON.
- At or below **28°C**, the fan switches OFF.
- Between these thresholds, the fan maintains its previous state.

This approach uses hysteresis to reduce frequent switching.

### Step 4: Gas-Level Warning

The MQ-7B raw analogue reading is monitored. When the reading reaches or exceeds the configured threshold of 2500, the warning LED is activated.

This threshold is a prototype trigger and is not a calibrated gas concentration in ppm.

### Step 5: Wireless Communication

The ESP32 communicates with the Flask backend through Wi-Fi, transmitting the collected sensor information.

### Step 6: Dashboard Monitoring

The Flask backend receives the readings and connects the hardware system to the web-based dashboard.

### Step 7: Machine Learning Integration

A Decision Tree model is trained using greenhouse environmental data, exported into C code and integrated into the embedded development environment.

## 9. Machine Learning Component

SmartCool incorporates a Decision Tree classifier for identifying environmental conditions associated with potential overheating.

### Dataset

The model was developed using:

**Temperature and Humidity Dataset of an East-Facing South African Greenhouse Tunnel**

Source: Mendeley Data

https://data.mendeley.com/datasets/54htxm94bv/1

### Input Features

The model uses six environmental features:

1. Front-zone temperature
2. Front-zone humidity
3. Middle-zone temperature
4. Middle-zone humidity
5. Back-zone temperature
6. Back-zone humidity

### Model Configuration

- Algorithm: Decision Tree Classifier
- Maximum depth: 5
- Random state: 42
- Class weighting: Balanced

### Model Performance

| Metric | Result |
|---|---:|
| Accuracy | 97.66% |
| Precision | 46.63% |
| Recall | 87.37% |
| F1-score | 60.81% |

The model was exported into `SmartCoolModel.h`, containing 59 nodes and the prediction function:

```cpp
predictSmartCool
