# 🏡 Distributed IoT Smart Home System (Edge-Cloud Architecture)

![C/C++](https://img.shields.io/badge/Language-C%2FC%2B%2B-blue.svg)
![STM32](https://img.shields.io/badge/MCU-STM32F103-blue.svg)
![ESP32](https://img.shields.io/badge/Gateway-ESP32--S3-red.svg)
![FreeRTOS](https://img.shields.io/badge/OS-FreeRTOS-green.svg)
![ThingsBoard](https://img.shields.io/badge/Cloud-ThingsBoard-0f2c5e.svg)

## 📖 Giới thiệu Dự án
Hệ thống Smart Home IoT phân tán (Distributed System) được thiết kế nhằm chia tách rõ ràng nhiệm vụ giữa phần cứng điều khiển cấp thấp (Edge) và viễn thông mạng (Gateway). Hệ thống đảm bảo tính an toàn, phản hồi theo thời gian thực (real-time) và khả năng giám sát, điều khiển từ xa thông qua giao thức MQTT.

Dự án được xây dựng trên sự kết hợp giữa **STM32** (chạy Bare-metal/Super-loop) và **ESP32-S3 YOLO UNO** (chạy hệ điều hành FreeRTOS).

---

## 🏗️ Kiến trúc Hệ thống (System Architecture)

Hệ thống được chia làm hai phân hệ chính, giao tiếp với nhau thông qua UART (JSON định dạng):

### 1. STM32 Edge Node (Quản lý Phần cứng)
* **Hoạt động cục bộ (Deterministic Automation):** Liên tục đọc dữ liệu từ cảm biến nhiệt độ, độ ẩm (DHT20 qua I2C) và cảm biến ánh sáng (LDR qua GPIO).
* **Xử lý logic tự động:** Trực tiếp đóng/cắt Relay (IN1, IN2, IN3) dựa trên các ngưỡng môi trường (VD: Nhiệt độ > 32°C bật quạt, trời tối bật đèn) độc lập hoàn toàn với kết nối mạng.
* **Ngắt UART (Interrupt-based):** Luôn sẵn sàng nhận lệnh từ ESP32 để điều khiển kênh Relay đặc quyền (IN4).

### 2. ESP32-S3 Gateway (Quản lý Đa luồng & Cloud)
* **FreeRTOS Task Management:** Sử dụng kiến trúc đa luồng để chạy song song Task đọc UART và Task kết nối MQTT. Các Task chia sẻ dữ liệu an toàn thông qua **Mutex (Semaphore)**.
* **Luồng Telemetry (Viễn trắc):** Nhận gói JSON từ STM32, bóc tách và đẩy dữ liệu môi trường (Nhiệt độ, Độ ẩm, Ánh sáng) lên máy chủ ThingsBoard.
* **Luồng RPC (Remote Procedure Call):** Lắng nghe lệnh từ Web Dashboard. Khi người dùng nhấn nút, ESP32 sẽ bắt lệnh RPC và gửi tín hiệu (`'4'` hoặc `'5'`) qua UART để ép STM32 đóng/cắt kênh IN4.

---

## 🛠️ Công nghệ & Giao thức

* **Ngôn ngữ:** C / C++
* **Hệ điều hành:** FreeRTOS (Task, Mutex)
* **Môi trường phát triển:** STM32CubeIDE (STM32 HAL), VS Code + PlatformIO (Arduino framework)
* **Giao thức truyền thông:** I2C, UART, MQTT, WiFi
* **IoT Platform:** ThingsBoard (Telemetry & Server-Side RPC)
* **Định dạng dữ liệu:** JSON (ArduinoJson)

---

## 🔌 Sơ đồ nối dây (Pinout & Wiring)

| Thiết bị / Module | Chân STM32 | Chân ESP32-S3 | Ghi chú |
| :--- | :--- | :--- | :--- |
| **Giao tiếp UART** | PA9 (TX) | D9 / GPIO 18 (RX) | Giao tiếp 2 chiều (Baudrate 9600) |
| **Giao tiếp UART** | PA10 (RX) | D8 / GPIO 17 (TX) | Giao tiếp 2 chiều |
| **GND Chung** | GND | GND | **Bắt buộc** để đồng bộ điện áp |
| **Cảm biến DHT20**| PB6 (SCL), PB7 (SDA) | - | Nguồn 3.3V cấp từ ESP32 |
| **Cảm biến LDR** | PA0 (Digital Read) | - | - |
| **Module Relay** | PA1, PA2, PA3, PA4 | - | Điều khiển IN1, IN2, IN3, IN4 |

---

## 📁 Cấu trúc Thư mục

```text
📦 Distributed_IoT_Gateway
 ┣ 📂 ESP32_Gateway        # Mã nguồn Gateway (PlatformIO)
 ┃ ┣ 📂 src
 ┃ ┃ ┣ 📜 main.cpp         # Khởi tạo FreeRTOS Mutex & Tasks
 ┃ ┃ ┣ 📜 global.cpp       # Biến chia sẻ toàn cục
 ┃ ┃ ┣ 📜 task_uart.cpp    # Task đọc/giải mã JSON từ UART
 ┃ ┃ ┗ 📜 task_mqtt.cpp    # Task kết nối WiFi, gửi Telemetry & nhận RPC
 ┃ ┣ 📜 platformio.ini     # Cấu hình board và thư viện (PubSubClient, ArduinoJson)
 ┃ ┗ 📂 include
 ┣ 📂 STM32_Node           # Mã nguồn Edge Node (STM32CubeIDE)
 ┃ ┣ 📂 Core
 ┃ ┃ ┣ 📂 Src
 ┃ ┃ ┃ ┣ 📜 main.c         # Super-loop & Hardware Timers
 ┃ ┃ ┃ ┣ 📜 dht20.c        # Thư viện I2C DHT20
 ┃ ┃ ┃ ┣ 📜 ldr.c          # Xử lý tự động hóa môi trường
 ┃ ┃ ┃ ┗ 📜 uart_com.c     # Đóng gói JSON & Xử lý ngắt nhận lệnh RPC
 ┃ ┃ ┗ 📂 Inc
 ┃ ┗ 📜 STM32_Node.ioc     # File cấu hình CubeMX (Timer 2, UART 1 Interrupts)