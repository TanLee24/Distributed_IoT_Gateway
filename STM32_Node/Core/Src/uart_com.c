/*
 * uart_com.c
 *
 *  Created on: Apr 12, 2026
 *      Author: LENOVO
 */

#include "uart_com.h"

extern UART_HandleTypeDef huart1;

/**
 * @brief Serializes the current environmental telemetry data into a JSON string
 * and transmits it to the edge gateway via UART.
 */
void UART_SendData(void)
{
    // Format the sensory variables into a compact, standardized JSON payload
    sprintf(tx_buffer, "{\"temp\":%.1f,\"humi\":%.1f,\"light\":%d}\n",
            temp, humi, light_val);

    // Dispatch the serialized payload via blocking UART transmission
    HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 100);
}

/**
 * @brief Processes incoming Remote Procedure Call (RPC) commands from the gateway.
 * This function is invoked within the UART RX Complete Callback interrupt context.
 */
void UART_ReceiveProcess(void)
{
    // Parse the received byte to execute exclusive remote actuation commands
    if (rx_data == '4')
    {
        // Command '4': Energize the server-controlled relay (IN4)
        HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_RESET);
    }
    else if (rx_data == '5')
    {
        // Command '5': De-energize the server-controlled relay (IN4)
        HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_SET);
    }

    // Re-arm the non-blocking UART receive interrupt to listen for subsequent commands
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
}

