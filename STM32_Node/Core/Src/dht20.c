/*
 * dht20.c
 *
 *  Created on: 12 Apr 2026
 *      Author: LENOVO
 */

#include "dht20.h"

extern I2C_HandleTypeDef hi2c1;

/**
 * @brief Reads temperature and humidity data from the DHT20 sensor via I2C.
 * The raw 20-bit data is parsed and converted into standard physical units.
 */
void DHT20_Read(void)
{
    // Initialization command sequence for the DHT20 sensor
    uint8_t cmd[3] = {0xAC, 0x33, 0x00};
    uint8_t data[7];

    // Transmit measurement command to the sensor (I2C Address: 0x38, shifted left by 1 -> 0x70)
    if(HAL_I2C_Master_Transmit(&hi2c1, 0x70, cmd, 3, 100) == HAL_OK)
    {
        // Allow the sensor sufficient time to complete environmental sampling
        HAL_Delay(80);

        // Retrieve 7 bytes of measurement data from the sensor
        if(HAL_I2C_Master_Receive(&hi2c1, 0x70, data, 7, 100) == HAL_OK)
        {
            // Check the status bit (bit 7 of data[0]) to ensure data readiness (0 = ready)
            if ((data[0] & 0x80) == 0)
            {
                // Concatenate byte segments to construct the 20-bit raw humidity value
                uint32_t humi_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);

                // Concatenate byte segments to construct the 20-bit raw temperature value
                uint32_t temp_raw = (((uint32_t)(data[3] & 0x0F)) << 16) | ((uint32_t)data[4] << 8) | data[5];

                // Convert raw digital values into physical units (Percentage and Celsius)
                humi = ((float)humi_raw / 1048576.0) * 100.0;
                temp = ((float)temp_raw / 1048576.0) * 200.0 - 50.0;
            }
        }
    }
}

/**
 * @brief Executes local deterministic automation logic based on environmental thresholds.
 * Controls the cooling and humidification peripherals autonomously.
 */
void Environment_Process(void)
{
    // Temperature threshold evaluation: Activate cooling fan (IN2) if temperature exceeds 32.0°C
    if (temp > 32.0)
    {
        HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET); // Energize relay (Active Low)
    }
    else
    {
        HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_SET);   // De-energize relay
    }

    // Humidity threshold evaluation: Activate humidifier (IN3) if humidity drops below 60%
    // Guard condition (humi > 0.0) prevents false triggering on sensor read failures
    if (humi > 0.0 && humi < 75.0)
    {
        HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET); // Energize relay (Active Low)
    }
    else
    {
        HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_SET);   // De-energize relay
    }
}
