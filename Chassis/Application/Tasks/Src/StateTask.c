//
// Created by YanYuanbin on 22-10-5.
//

#include "cmsis_os.h"

#include "StateTask.h"
#include "INS_Task.h"


#include "remote_control.h"
#include "bsp_can.h"
#include "tim.h"

//系统状态信息结构体，待完善
System_Info_t System_Info={
		.state = Disable,
};

/* Private function prototypes -----------------------------------------------*/
static void rc_ctrl_monitor(rc_ctrl_t *rc_ctrl);
//static void rc_communicate(rc_ctrl_t *rc_ctrl,CAN_TxFrameTypeDef *TxHeader);

/* USER CODE BEGIN Header_StateTask */
/**
* @brief Function implementing the myStateTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StateTask */
void StateTask(void const * argument)
{
  /* USER CODE BEGIN SerialTask */
	
  /* Infinite loop */
  for(;;)
  {
		
//遥控器状态机
 	rc_ctrl_monitor(&rc_ctrl);

////遥控器数据共享
//		rc_communicate(&rc_ctrl,&RBCTxFrame);
//		
    osDelay(2);
  }
  /* USER CODE END SerialTask */
}


/**
  * @brief          系统状态机
  * @param[out]     systemstate:系统状态信息变量指针.
  * @retval         none
  */
//static void system_monitor(System_Info_t *systemstate)
//{
//	if(systemstate ==  NULL)
//	{
//		return;
//	}

////待完善
//}

/**
  * @brief          遥控器数据保护
  * @param[out]     rc_ctrl_offline:遥控器信息变量指针.
  * @retval         none
  */
static void rc_ctrl_protect(rc_ctrl_t *rc_ctrl_offline)
{
	if(rc_ctrl_offline == NULL)
	{
		return;
	}
//清空离线的遥控器数据
	memset(rc_ctrl_offline,0,sizeof(*rc_ctrl_offline));
}

/**
  * @brief          遥控器状态机
  * @param[out]     rc_ctrl:遥控器信息变量指针.
  * @retval         none
  */
static void rc_ctrl_monitor(rc_ctrl_t *rc_ctrl)
{
	if(rc_ctrl == NULL)
	{
		return;
	}
//计数小于50，认为遥控器离线
	if(rc_ctrl->online_Cnt <= 50)
	{
		rc_ctrl->lost = 1;
//遥控器数据保护
		rc_ctrl_protect(rc_ctrl);
	}else
	{
		rc_ctrl->lost = 0;		
	}
//计数自减，在接收中断内重新赋值
	if(rc_ctrl->online_Cnt > 0)
	{
		rc_ctrl->online_Cnt--;
	}
}

/**
  * @brief          遥控器数据共享
	* @param[out]     rc_ctrl:遥控器信息变量指针, TxHeader:CAN发送帧包变量指针
  * @retval         none
  */
//static void rc_communicate(rc_ctrl_t *rc_ctrl,CAN_TxFrameTypeDef *TxHeader)
//{
//	if(rc_ctrl == NULL || TxHeader == NULL) return;
//	
//	TxHeader->data[0] = (uint8_t)(rc_ctrl->rc.s[0] << 6) | (uint8_t)(rc_ctrl->rc.s[1] << 4) | 
//											(uint8_t)(rc_ctrl->lost << 3 )   | (uint8_t)(rc_ctrl->rc.ch[4]==660) << 2;
//	
//	TxHeader->data[2] = (uint8_t)(rc_ctrl->rc.ch[3] >> 8);
//	TxHeader->data[3] = (uint8_t)(rc_ctrl->rc.ch[3]);
//	TxHeader->data[4] = (uint8_t)(rc_ctrl->rc.ch[2] >> 8);
//	TxHeader->data[5] = (uint8_t)(rc_ctrl->rc.ch[2]);
//	TxHeader->data[6] = (uint8_t)(rc_ctrl->key.v >> 8);
//	TxHeader->data[7] = (uint8_t)(rc_ctrl->key.v);

//	USER_CAN_TxMessage(TxHeader);
//}
