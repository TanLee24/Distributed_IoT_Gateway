/*
 * ldr.c
 *
 *  Created on: 12 Apr 2026
 *      Author: LENOVO
 */

#include "ldr.h"

/**
 * @brief Processes the digital signal from the Light Dependent Resistor (LDR) module.
 * Automatically toggles the designated lighting relay based on ambient brightness.
 */
void LDR_Process(void)
{
    // Poll the digital input state of the LDR sensor
    if(HAL_GPIO_ReadPin(DO_GPIO_Port, DO_Pin) == GPIO_PIN_SET)
    {
        // High logic state indicates insufficient ambient light (Darkness)
        light_val = 1;

        // Energize the lighting relay (Active Low configuration)
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    }
    else
    {
        // Low logic state indicates sufficient ambient light (Daylight)
        light_val = 0;

        // De-energize the lighting relay
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    }
}
