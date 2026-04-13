#include "global.h"
#include "task_uart.h"
#include "task_mqtt.h"

void setup() 
{
    Serial.begin(115200);
    Serial.println("=== ESP32-S3 RTOS GATEWAY ===");

    dataMutex = xSemaphoreCreateMutex();
    if (dataMutex == NULL) 
    {
        Serial.println("Lỗi: Không tạo được Mutex!");
        while (1); 
    }

    xTaskCreate(Task_UART_Process, "UART_Task", 4096, NULL, 1, NULL);
    xTaskCreate(Task_MQTT_Process, "MQTT_Task", 8192, NULL, 1, NULL);
}

void loop() 
{
    vTaskDelete(NULL);
}