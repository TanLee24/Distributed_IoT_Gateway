#ifndef __TASK_UART_H__
#define __TASK_UART_H__

#include "global.h"
#include <ArduinoJson.h>

#define RX_PIN 44
#define TX_PIN 43

void Task_UART_Process(void *pvParameters);

#endif