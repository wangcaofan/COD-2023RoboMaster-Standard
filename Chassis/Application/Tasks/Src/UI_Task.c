#include "UI_Task.h"


void UI_Task(void const * argument)
{
  /* USER CODE BEGIN UI_Task */
  /* Infinite loop */
  for(;;)
  {
		Startjudge_task();
    osDelay(5);
  }
  /* USER CODE END UI_Task */
}
