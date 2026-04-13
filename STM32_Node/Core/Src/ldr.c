/*
 * ldr.c
 *
 *  Created on: 12 Apr 2026
 *      Author: LENOVO
 */

#include "ldr.h"

void LDR_Process(void) {
    // Đọc chân PA0
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
        light_val = 1; // Tối
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    } else {
        light_val = 0; // Sáng
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // Tắt
    }

}
