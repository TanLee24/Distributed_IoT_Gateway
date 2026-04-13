/*
 * uart_com.c
 *
 *  Created on: Apr 12, 2026
 *      Author: LENOVO
 */

#include "uart_com.h"

extern UART_HandleTypeDef huart1;

void UART_SendData(void) {
    // Đóng gói JSON mới (Không còn biến "mode")
    sprintf(tx_buffer, "{\"temp\":%.1f,\"humi\":%.1f,\"light\":%d}\n",
            temp, humi, light_val);

    HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 100);
}

void UART_ReceiveProcess(void) {
    // ESP32 chỉ được quyền can thiệp vào IN4
    if (rx_data == '4') {
        HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_RESET); // Bật IN4
    }
    else if (rx_data == '5') {
        HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_SET);   // Tắt IN4
    }

    // Mở lại ngắt nhận UART
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
}
