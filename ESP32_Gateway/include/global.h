#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// The struct aggregates all system data
struct SensorData 
{
    float temp;
    float humi;
    int light;
};

// Declare extern so that other files can call it.
extern SensorData sharedData;
extern SemaphoreHandle_t dataMutex; // Mutex lock

#endif