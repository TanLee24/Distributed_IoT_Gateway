#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Cấu trúc gom toàn bộ dữ liệu hệ thống
struct SensorData {
    float temp;
    float humi;
    int light;
};

// Khai báo extern để các file khác gọi được
extern SensorData sharedData;
extern SemaphoreHandle_t dataMutex; // Ổ khóa Mutex

#endif