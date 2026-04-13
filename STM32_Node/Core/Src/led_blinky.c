/*
 * led_blinky.c
 *
 *  Created on: 12 Apr 2026
 *      Author: LENOVO
 */

#include "led_blinky.h"

void ledBlinky()
{
	HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
}
