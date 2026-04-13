#include <Arduino.h>
#include "global.h"
#include "task_uart.h"
#include "mqtt.h"

void setup() {
    Serial.begin(115200);
    
    Serial.println("=== ESP32-S3 RTOS GATEWAY ===");

    // 1. Khởi tạo Ổ khóa Mutex
    dataMutex = xSemaphoreCreateMutex();
    if (dataMutex == NULL) {
        Serial.println("Lỗi: Không tạo được Mutex!");
        while (1); 
    }

    // 2. Tạo Task UART (Chạy trên Core 1 - Xử lý tính toán)
    xTaskCreate(
        Task_UART_Process,  // Tên hàm
        "UART_Task",        // Tên gợi nhớ
        4096,               // Kích thước RAM (Bytes)
        NULL,               // Tham số truyền vào
        1,                  // Độ ưu tiên (1-24)
        NULL                // Con trỏ quản lý Tas
    );

    xTaskCreate(
        Task_MQTT_Process,
        "MQTT_Task",
        8192,               
        NULL,
        1,                  
        NULL
    );
}

void loop() {
    vTaskDelete(NULL);
}