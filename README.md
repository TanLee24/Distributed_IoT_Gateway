# 🏡 Distributed IoT Smart Home System (Edge-Cloud Architecture)

![C/C++](https://img.shields.io/badge/Language-C%2FC%2B%2B-blue.svg)
![STM32](https://img.shields.io/badge/MCU-STM32F103-blue.svg)
![ESP32](https://img.shields.io/badge/Gateway-ESP32--S3-red.svg)
![FreeRTOS](https://img.shields.io/badge/OS-FreeRTOS-green.svg)
![ThingsBoard](https://img.shields.io/badge/Cloud-ThingsBoard-0f2c5e.svg)

## 📖 Project Overview
This Distributed IoT Smart Home System is architected to explicitly decouple low-level hardware execution (Edge Node) from network communication and cloud integration (Gateway). The system guarantees fail-safe deterministic operation, real-time responsiveness, and remote monitoring/control capabilities via the MQTT protocol.

The project leverages the combined processing power of an **STM32** microcontroller (running a Bare-metal Super-loop) and an **ESP32-S3 YOLO UNO** board (running FreeRTOS).

---

## 🏗️ System Architecture

The system is divided into two primary subsystems, communicating continuously via a JSON-over-UART protocol:

### 1. STM32 Edge Node (Hardware Manager)
* **Deterministic Local Automation:** Continuously acquires data from temperature/humidity sensors (DHT20 via I2C) and a light sensor (LDR via Digital Input).
* **Autonomous Logic Execution:** Directly controls solid-state/mechanical Relays (IN1, IN2, IN3) based on environmental thresholds (e.g., triggering a fan if Temp > 32°C, or turning on lights when dark) completely independent of network connectivity.
* **Interrupt-Driven UART:** Constantly listens for incoming commands from the ESP32 Gateway to control the exclusive Server-Side RPC Relay (IN4) without blocking the main loop.

### 2. ESP32-S3 Gateway (Concurrent Task & Cloud Manager)
* **FreeRTOS Task Management:** Utilizes a multi-threaded architecture to run UART processing and MQTT network handling concurrently. Data integrity between tasks is strictly maintained using **Mutexes (Semaphores)**.
* **Telemetry Streaming (Uplink):** Parses the JSON payload from the STM32 and publishes environmental data (Temperature, Humidity, Light State) to the ThingsBoard Cloud platform.
* **Server-Side RPC (Downlink):** Subscribes to the RPC topic from the Web Dashboard. Upon receiving a user-triggered command, the ESP32 captures the RPC request and transmits the corresponding actuation signal (`'4'` or `'5'`) via UART to force the STM32 to toggle the IN4 Relay.

---

## 🛠️ Technologies & Protocols

* **Languages:** C / C++
* **Operating System:** FreeRTOS (Tasks, Mutexes)
* **Development Environments:** STM32CubeIDE (STM32 HAL), VS Code + PlatformIO (Arduino Framework)
* **Communication Protocols:** I2C, UART, MQTT, Wi-Fi
* **IoT Platform:** ThingsBoard (Telemetry & Server-Side RPC)
* **Data Serialization:** JSON (ArduinoJson)

---

## 🔌 Pinout & Wiring Diagram

| Component / Module | STM32 Pin | ESP32-S3 Pin | Notes |
| :--- | :--- | :--- | :--- |
| **UART TX/RX** | PA9 (TX) | D9 / GPIO 18 (RX) | Bi-directional communication (Baudrate: 9600) |
| **UART RX/TX** | PA10 (RX) | D8 / GPIO 17 (TX) | Bi-directional communication |
| **Common Ground** | GND | GND | **Mandatory** for voltage level synchronization |
| **DHT20 Sensor**| PB6 (SCL), PB7 (SDA) | - | 3.3V Power supplied from ESP32 |
| **LDR Sensor** | PA0 (Digital IN) | - | - |
| **Relay Module** | PA1, PA2, PA3, PA4 | - | Controls IN1, IN2, IN3, IN4 |

---

## 📁 Directory Structure

```text
📦 Distributed_IoT_Gateway
 ┣ 📂 ESP32_Gateway        # Gateway Source Code (PlatformIO)
 ┃ ┣ 📂 src
 ┃ ┃ ┣ 📜 main.cpp         # FreeRTOS Mutex & Task Initialization
 ┃ ┃ ┣ 📜 global.cpp       # Global shared variables
 ┃ ┃ ┣ 📜 task_uart.cpp    # Task: Read/Deserialize JSON from UART
 ┃ ┃ ┗ 📜 task_mqtt.cpp    # Task: Wi-Fi/MQTT connection, Telemetry & RPC
 ┃ ┣ 📜 platformio.ini     # Board & Library configurations (PubSubClient, ArduinoJson)
 ┃ ┗ 📂 include
 ┣ 📂 STM32_Node           # Edge Node Source Code (STM32CubeIDE)
 ┃ ┣ 📂 Core
 ┃ ┃ ┣ 📂 Src
 ┃ ┃ ┃ ┣ 📜 main.c         # Super-loop & Hardware Timer Callbacks
 ┃ ┃ ┃ ┣ 📜 dht20.c        # DHT20 I2C Driver
 ┃ ┃ ┃ ┣ 📜 ldr.c          # Local environmental automation logic
 ┃ ┃ ┃ ┗ 📜 uart_com.c     # JSON Serialization & UART RX Interrupt handler
 ┃ ┃ ┗ 📂 Inc
 ┃ ┗ 📜 STM32_Node.ioc     # CubeMX Configuration (Timer 2, USART1 Interrupts)