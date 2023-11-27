//
// Created by YanYuanbin on 22-10-4.
//
#include "bsp_can.h"
#include "motor.h"
#include "ControlTask.h"
CAN_RxFrameTypeDef hcanRxFrame;
CAN_TxFrameTypeDef hcan1TxFrame = {
		.hcan = &hcan1,
		.header.StdId=0x1ff,
		.header.IDE=CAN_ID_STD,
		.header.RTR=CAN_RTR_DATA,
		.header.DLC=8,
};
CAN_TxFrameTypeDef GimbalTxFrame = {
		.hcan = &hcan1,
		.header.StdId=0x1ff,
		.header.IDE=CAN_ID_STD,
		.header.RTR=CAN_RTR_DATA,
		.header.DLC=8,
		};
CAN_TxFrameTypeDef ShootTxFrame = {
		.hcan = &hcan2,
		.header.StdId=0x200,
		.header.IDE=CAN_ID_STD,
		.header.RTR=CAN_RTR_DATA,
		.header.DLC=8,
		};
CAN_TxFrameTypeDef Send2ChassisTxFrame = {
		.hcan = &hcan1,
		.header.StdId=0x101,
		.header.IDE=CAN_ID_STD,
		.header.RTR=CAN_RTR_DATA,
		.header.DLC=8,
};
void CAN_Init(void)
{
    CAN_FilterTypeDef sFilterConfig={0};

		sFilterConfig.FilterActivation = ENABLE;
		sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
		sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
		sFilterConfig.FilterIdHigh = 0x0000;
		sFilterConfig.FilterIdLow = 0x0000;
		sFilterConfig.FilterMaskIdHigh = 0x0000;
		sFilterConfig.FilterMaskIdLow = 0x0000;
		sFilterConfig.FilterBank = 0;
		sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
		sFilterConfig.SlaveStartFilterBank = 0;

		// ??CAN??????
		if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
		{	
		    Error_Handler();
		}
    // ??CAN1
    HAL_CAN_Start(&hcan1);
		// ??????
		HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
		
		sFilterConfig.SlaveStartFilterBank = 14;
    // ??CAN2
    HAL_CAN_Start(&hcan2);
		// ??????
		HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);

}
/**
  * @brief          ???CAN??
  * @param[out]     TxHeader:CAN????????.
  * @retval         none
  */
void USER_CAN_TxMessage(CAN_TxFrameTypeDef *TxHeader)
{
	if(TxHeader == NULL || TxHeader->hcan == NULL) return;
	
	uint32_t TxMailbox = 0;

//   while( HAL_CAN_GetTxMailboxesFreeLevel( &hcan1 ) == 0 );
	//???????CAN??
	HAL_CAN_AddTxMessage(TxHeader->hcan, &TxHeader->header, TxHeader->data, &TxMailbox);
}

/**
 *	@brief	CAN1??????
 */
void CAN1_rxDataHandler(uint32_t *canId, uint8_t *rxBuf)
{
	if(canId == NULL || rxBuf == NULL) return;
		get_Motor_Data(canId,rxBuf,&Gimbal_Motor[Yaw]);
	  get_Motor_Data(canId,rxBuf,&Gimbal_Motor[Pitch]);
	  if( *canId == 0x100){
		  Chassis_Message.shoot_remainingHeat = (int16_t)(rxBuf[0]<<8) | (int16_t)(rxBuf[1]);
		  Chassis_Message.shoot_speedGain     = (int16_t)(rxBuf[2]<<8) | (int16_t)(rxBuf[3]);
			Chassis_Message.shoot_expectSpeed   = (int16_t)(rxBuf[4]<<8) | (int16_t)(rxBuf[5]);
			Chassis_Message.Chassis_Yaw_gyro    = (int16_t)(rxBuf[6] << 8 | rxBuf[7]) /100.f;
		}
}

//??pitch?,????pitch????,????

float chassis_pitangle = 0;
/**
 *	@brief	CAN2??????
 */
void CAN2_rxDataHandler(uint32_t *canId, uint8_t *rxBuf)
{
	if(canId == NULL || rxBuf == NULL) return;
		get_Motor_Data(canId,rxBuf,&Gimbal_Motor[SHOOTL]);
	  get_Motor_Data(canId,rxBuf,&Gimbal_Motor[SHOOTR]);
	  get_Motor_Data(canId,rxBuf,&Gimbal_Motor[TRIGGER]);
}
/**
 *	@brief	?? CAN RxFifo ??????
 *	@note	?stm32f4xx_hal_can.c????
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{		
    // CAN1 ????
    if(hcan->Instance == CAN1)
    {
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hcanRxFrame.header, hcanRxFrame.data);
        CAN1_rxDataHandler(&hcanRxFrame.header.StdId,hcanRxFrame.data);
		}
    // CAN2 ????
    else if(hcan->Instance == CAN2)
    {
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hcanRxFrame.header, hcanRxFrame.data);
        CAN2_rxDataHandler(&hcanRxFrame.header.StdId,hcanRxFrame.data);
		}
}
