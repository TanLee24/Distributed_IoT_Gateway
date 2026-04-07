/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "global.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
I2C_HandleTypeDef hi2c1;
ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart1;

#define DHT20_ADDR 0x70
float shared_temp = 0;
float shared_humi = 0;
uint16_t shared_light = 0;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId SensorsHandle;
osThreadId CommunicationHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void DHT20_Read(void) {
    uint8_t cmd[3] = {0xAC, 0x33, 0x00};
    uint8_t data[7];

    HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDR, cmd, 3, 100);
    osDelay(80);

    HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDR, data, 7, 100);

    if ((data[0] & 0x80) == 0) {
        uint32_t humi_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
        uint32_t temp_raw = (((uint32_t)(data[3] & 0x0F)) << 16) | ((uint32_t)data[4] << 8) | data[5];

        shared_humi = ((float)humi_raw / 1048576.0) * 100.0;
        shared_temp = ((float)temp_raw / 1048576.0) * 200.0 - 50.0;
    }
}

uint16_t LDR_Read(void) {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint16_t val = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    return (100 - (val * 100 / 4095));
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void taskSensors(void const * argument);
void taskCommunication(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Sensors */
  osThreadDef(Sensors, taskSensors, osPriorityNormal, 0, 512);
  SensorsHandle = osThreadCreate(osThread(Sensors), NULL);

  /* definition and creation of Communication */
  osThreadDef(Communication, taskCommunication, osPriorityNormal, 0, 512);
  CommunicationHandle = osThreadCreate(osThread(Communication), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_taskSensors */
/**
* @brief Function implementing the Sensors thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_taskSensors */
void taskSensors(void const * argument)
{
  /* USER CODE BEGIN taskSensors */
  /* Infinite loop */
  for(;;)
  {
	  DHT20_Read();
	  shared_light = LDR_Read();
	  osDelay(2000);
  }
  /* USER CODE END taskSensors */
}

/* USER CODE BEGIN Header_taskCommunication */
/**
* @brief Function implementing the Communication thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_taskCommunication */
void taskCommunication(void const * argument)
{
  /* USER CODE BEGIN taskCommunication */
	char tx_buffer[100];
  /* Infinite loop */
	for(;;)
	{
		sprintf(tx_buffer, "{\"temp\": %.1f, \"humi\": %.1f, \"light\": %d}\n", shared_temp, shared_humi, shared_light);
		HAL_UART_Transmit_DMA(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer));
		osDelay(2000);
	}
  /* USER CODE END taskCommunication */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
