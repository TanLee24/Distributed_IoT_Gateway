/*
 * dht20.c
 *
 *  Created on: 12 Apr 2026
 *      Author: LENOVO
 */

#include "dht20.h"

extern I2C_HandleTypeDef hi2c1;

void DHT20_Read(void) {
    uint8_t cmd[3] = {0xAC, 0x33, 0x00};
    uint8_t data[7];
    // Gửi lệnh đo (0x70 là địa chỉ DHT20)
    if(HAL_I2C_Master_Transmit(&hi2c1, 0x70 << 1, cmd, 3, 100) == HAL_OK) {
        HAL_Delay(80); // Chờ đo
        if(HAL_I2C_Master_Receive(&hi2c1, 0x70 << 1, data, 7, 100) == HAL_OK) {
            if ((data[0] & 0x80) == 0) {
                uint32_t humi_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
                uint32_t temp_raw = (((uint32_t)(data[3] & 0x0F)) << 16) | ((uint32_t)data[4] << 8) | data[5];
                humi = ((float)humi_raw / 1048576.0) * 100.0;
                temp = ((float)temp_raw / 1048576.0) * 200.0 - 50.0;
            }
        }
    }
}

void Environment_Process(void) {
    // 1. Logic Nhiệt độ -> Điều khiển thẳng IN2
    if (temp > 32.0) {
        HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET); // Bật
    } else {
        HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_SET);   // Tắt
    }

    // 2. Logic Độ ẩm -> Điều khiển thẳng IN3
    if (humi > 0.0 && humi < 60.0) {
        HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET); // Bật
    } else {
        HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_SET);   // Tắt
    }
}
