/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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

/* USER CODE END Variables */
osThreadId StartINSTaskTASHandle;
osThreadId myChassis_TaskHandle;
osThreadId myCANTx_TaskHandle;
osThreadId myUITaskHandle;
osThreadId myREFEREE_TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void INS_Task(void const * argument);
void Chassis_Task(void const * argument);
void CANTx_Task(void const * argument);
void UI_Task(void const * argument);
void Referee_Task(void const * argument);

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
  /* definition and creation of StartINSTaskTAS */
  osThreadDef(StartINSTaskTAS, INS_Task, osPriorityRealtime, 0, 256);
  StartINSTaskTASHandle = osThreadCreate(osThread(StartINSTaskTAS), NULL);

  /* definition and creation of myChassis_Task */
  osThreadDef(myChassis_Task, Chassis_Task, osPriorityRealtime, 0, 256);
  myChassis_TaskHandle = osThreadCreate(osThread(myChassis_Task), NULL);

  /* definition and creation of myCANTx_Task */
  osThreadDef(myCANTx_Task, CANTx_Task, osPriorityRealtime, 0, 256);
  myCANTx_TaskHandle = osThreadCreate(osThread(myCANTx_Task), NULL);

  /* definition and creation of myUITask */
  osThreadDef(myUITask, UI_Task, osPriorityRealtime, 0, 256);
  myUITaskHandle = osThreadCreate(osThread(myUITask), NULL);

  /* definition and creation of myREFEREE_Task */
  osThreadDef(myREFEREE_Task, Referee_Task, osPriorityRealtime, 0, 256);
  myREFEREE_TaskHandle = osThreadCreate(osThread(myREFEREE_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_INS_Task */
/**
  * @brief  Function implementing the StartINSTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_INS_Task */
__weak void INS_Task(void const * argument)
{
  /* USER CODE BEGIN INS_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END INS_Task */
}

/* USER CODE BEGIN Header_Chassis_Task */
/**
* @brief Function implementing the myChassis_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Chassis_Task */
__weak void Chassis_Task(void const * argument)
{
  /* USER CODE BEGIN Chassis_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Chassis_Task */
}

/* USER CODE BEGIN Header_CANTx_Task */
/**
* @brief Function implementing the myCANTx_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CANTx_Task */
__weak void CANTx_Task(void const * argument)
{
  /* USER CODE BEGIN CANTx_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END CANTx_Task */
}

/* USER CODE BEGIN Header_UI_Task */
/**
* @brief Function implementing the myUITask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_UI_Task */
__weak void UI_Task(void const * argument)
{
  /* USER CODE BEGIN UI_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END UI_Task */
}

/* USER CODE BEGIN Header_Referee_Task */
/**
* @brief Function implementing the myREFEREE_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Referee_Task */
__weak void Referee_Task(void const * argument)
{
  /* USER CODE BEGIN Referee_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Referee_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
