#include "task_uart.h"

void Task_UART_Process(void *pvParameters) 
{
    // Initialize Serial1 for UART communication
    Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Set baud rate to 9600 to match STM32 configuration
    
    // Infinite loop for the RTOS task
    while (1) 
    {
        if (Serial1.available()) 
        {
            String incomingData = Serial1.readStringUntil('\n');
            incomingData.trim();
            
            if (incomingData.length() > 0) 
            {
                Serial.print("[Raw Data] Received: ");
                Serial.println(incomingData);

                JsonDocument doc; 
                DeserializationError error = deserializeJson(doc, incomingData);

                if (!error) 
                {
                    // Attempt to acquire Mutex with a maximum block time of 100 ticks
                    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) 
                    {
                        // Mutex successfully acquired, proceed to update shared data
                        sharedData.temp = doc["temp"] | 0.0;
                        sharedData.humi = doc["humi"] | 0.0;
                        sharedData.light = doc["light"] | 0;                        
                        Serial.println("[UART Task] Shared data updated from STM32");
                        
                        // Release Mutex after data update
                        xSemaphoreGive(dataMutex);
                    }
                }
                else
                {
                    Serial.print("❌ JSON Parse Error: ");
                    Serial.println(error.c_str());
                }
            }
        }
        // Task delay to yield CPU resources to other tasks
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}