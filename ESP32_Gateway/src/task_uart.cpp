#include "task_uart.h"
#include "global.h"
#include <ArduinoJson.h>

#define RX_PIN 44
#define TX_PIN 43

// #define RX_PIN 18 // D9
// #define TX_PIN 17 // D8

void Task_UART_Process(void *pvParameters) {
    // Khởi tạo cổng Serial1 cho UART
    Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Đang để 9600 để khớp với STM32
    
    // Vòng lặp vô tận của Task
    while (1) {
        if (Serial1.available()) {
            String incomingData = Serial1.readStringUntil('\n');
            incomingData.trim();
            
            if (incomingData.length() > 0) {
                Serial.print("[Raw Data] Nhận được: ");
                Serial.println(incomingData);

                JsonDocument doc; 
                DeserializationError error = deserializeJson(doc, incomingData);

                if (!error) {
                    // Xin chìa khóa (Lock Mutex) chờ tối đa 100 Ticks
                    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                        
                        // Đã khóa thành công, tiến hành ghi dữ liệu mới
                        sharedData.temp = doc["temp"] | 0.0;
                        sharedData.humi = doc["humi"] | 0.0;
                        sharedData.light = doc["light"] | 0;                        
                        Serial.println("[UART Task] Đã cập nhật dữ liệu từ STM32");
                        
                        // Xong việc phải trả lại chìa khóa (Unlock)
                        xSemaphoreGive(dataMutex);
                    }
                }
                else
                {
                    Serial.print("❌ Lỗi JSON: ");
                    Serial.println(error.c_str());
                }
            }
        }
        // Cho Task nghỉ ngơi một chút để nhường CPU
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}