#include "global.h"

SensorData sharedData = {0.0, 0.0, 0};
SemaphoreHandle_t dataMutex = NULL;