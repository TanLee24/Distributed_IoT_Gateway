#include "global.h"

// Cấp phát bộ nhớ thực tế
SensorData sharedData = {0.0, 0.0, 0};
SemaphoreHandle_t dataMutex = NULL;