#ifndef __MQTT_H__
#define __MQTT_H__

#include "global.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

void Task_MQTT_Process(void *pvParameters);

#endif