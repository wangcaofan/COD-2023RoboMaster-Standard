//
// Created by YanYuanbin on 22-10-5.
//

#include "cmsis_os.h"

#include "StateTask.h"
#include "INS_Task.h"


#include "remote_control.h"
#include "bsp_can.h"
#include "tim.h"

//ϵͳ״̬��Ϣ�ṹ�壬������
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
		
//ң����״̬��
 	rc_ctrl_monitor(&rc_ctrl);

////ң�������ݹ���
//		rc_communicate(&rc_ctrl,&RBCTxFrame);
//		
    osDelay(2);
  }
  /* USER CODE END SerialTask */
}


/**
  * @brief          ϵͳ״̬��
  * @param[out]     systemstate:ϵͳ״̬��Ϣ����ָ��.
  * @retval         none
  */
//static void system_monitor(System_Info_t *systemstate)
//{
//	if(systemstate ==  NULL)
//	{
//		return;
//	}

////������
//}

/**
  * @brief          ң�������ݱ���
  * @param[out]     rc_ctrl_offline:ң������Ϣ����ָ��.
  * @retval         none
  */
static void rc_ctrl_protect(rc_ctrl_t *rc_ctrl_offline)
{
	if(rc_ctrl_offline == NULL)
	{
		return;
	}
//������ߵ�ң��������
	memset(rc_ctrl_offline,0,sizeof(*rc_ctrl_offline));
}

/**
  * @brief          ң����״̬��
  * @param[out]     rc_ctrl:ң������Ϣ����ָ��.
  * @retval         none
  */
static void rc_ctrl_monitor(rc_ctrl_t *rc_ctrl)
{
	if(rc_ctrl == NULL)
	{
		return;
	}
//����С��50����Ϊң��������
	if(rc_ctrl->online_Cnt <= 50)
	{
		rc_ctrl->lost = 1;
//ң�������ݱ���
		rc_ctrl_protect(rc_ctrl);
	}else
	{
		rc_ctrl->lost = 0;		
	}
//�����Լ����ڽ����ж������¸�ֵ
	if(rc_ctrl->online_Cnt > 0)
	{
		rc_ctrl->online_Cnt--;
	}
}

/**
  * @brief          ң�������ݹ���
	* @param[out]     rc_ctrl:ң������Ϣ����ָ��, TxHeader:CAN����֡������ָ��
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
