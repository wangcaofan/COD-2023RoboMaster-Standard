#include "cmsis_os.h"

#include "CANTxTask.h"
#include "INS_Task.h"
#include "StateTask.h"
#include "ControlTask.h"
#include "bsp_can.h"
#include "motor.h"

#include "pid.h"
#include "kalman.h"
//float f_gimbal_Pid_Para[2][2][6]={
//	[0]={
//		[0]={0.f,0.f,5000.f,83.f,0.f,0.f,},
//		[1]={0.f,5000.f,30000.f,-40.f,0.17f,0.f,},
//	},
//	[1]={
//		[0]={0.f,0.f,5000.f,60.f,0.f,0.f,},
//		[1]={0,5000,30000,80,0.12f,0.f,},
//	},
//};

//PID
//PID_TypeDef_t Gimbal_PID[2][2];

////���ٶȿ������˲�
//static extKalman_t gyro_Kalman[2];

////��̨������Ϣ
//Gimbal_Info_t gimbal_Ctrl={
//		.rc_ctrl = &rc_ctrl,
//		.Measure.pit_angle = &INS.pit_angle,
//		.Measure.yaw_angle = &INS.yaw_tolangle,
//		.Measure.pit_gyro  = &INS.pit_gyro,
//		.Measure.yaw_gyro  = &INS.yaw_gyro,
////		.Limit_pitch.max = 30.f,
////		.Limit_pitch.min = -24.f,
//};


/* Private function prototypes -----------------------------------------------*/
//static void gimbal_init(void);
//static void gimbal_info_update(Gimbal_Info_t *Gimbal_Info_update);
//static void gimbal_posture_ctrl(Gimbal_Info_t *Gimbal_Info_control);
//static void chassis_send_current(Gimbal_Info_t *Gimbal_Info_send,CAN_TxFrameTypeDef *TXFrame);
void CANTx_Task(void const * argument)
{
	osDelay(1000);
	TickType_t systick = 0;
  /* USER CODE BEGIN ChassisTask */
	
	for(;;)
  {    
	 systick = osKernelSysTick();
  GimbalTxFrame.data[0] = (uint8_t)(gimbal_Ctrl.SendValue[Yaw] >> 8);
	GimbalTxFrame.data[1] =  (uint8_t)(gimbal_Ctrl.SendValue[Yaw]);
  GimbalTxFrame.data[2] = (uint8_t)(gimbal_Ctrl.SendValue[Pitch]>>8);
  GimbalTxFrame.data[3] = (uint8_t)(gimbal_Ctrl.SendValue[Pitch]);
	USER_CAN_TxMessage(&GimbalTxFrame);
	
  ShootTxFrame.data[0] = (uint8_t)(Shoot_Ctrl.SendValue[SHOOTL] >> 8);
	ShootTxFrame.data[1] = (uint8_t)(Shoot_Ctrl.SendValue[SHOOTL]);
	ShootTxFrame.data[2] = (uint8_t)(Shoot_Ctrl.SendValue[SHOOTR] >> 8);
	ShootTxFrame.data[3] = (uint8_t)(Shoot_Ctrl.SendValue[SHOOTR]);
	ShootTxFrame.data[4] = (uint8_t)(Shoot_Ctrl.SendValue[TRIGGER] >> 8);
	ShootTxFrame.data[5] = (uint8_t)(Shoot_Ctrl.SendValue[TRIGGER]);
	USER_CAN_TxMessage(&ShootTxFrame);
		
  Send2ChassisTxFrame.data[0] = Send2chassis_Message.ChassisWork;
  Send2ChassisTxFrame.data[1] = Send2chassis_Message.ChassisMode;		
	Send2ChassisTxFrame.data[2]	= (uint8_t)(Send2chassis_Message.vx >> 8);
  Send2ChassisTxFrame.data[3]	= (uint8_t)(Send2chassis_Message.vx);
  Send2ChassisTxFrame.data[4]	= (uint8_t)(Send2chassis_Message.vy >> 8);
 	Send2ChassisTxFrame.data[5] = (uint8_t)(Send2chassis_Message.vy);
  Send2ChassisTxFrame.data[6]	= Send2chassis_Message.SupercapWork;
  Send2ChassisTxFrame.data[7]	= Shoot_Ctrl.Workmode;
  USER_CAN_TxMessage(&Send2ChassisTxFrame);   
	osDelayUntil(&systick,1);
  }
  /* USER CODE END ChassisTask */
}


/**
  * @brief          ��̨��ʼ��
  * @param[out]     none
  * @retval         none
  */
//static void gimbal_init(void)
//{
////	//PID Init
////	PID_Init(&Gimbal_PID[Pitch][0],f_gimbal_Pid_Para[Pitch][0]);
////	PID_Init(&Gimbal_PID[Pitch][1],f_gimbal_Pid_Para[Pitch][1]);
////	PID_Init(&Gimbal_PID[Yaw][0]  ,f_gimbal_Pid_Para[Yaw][0]);
////	PID_Init(&Gimbal_PID[Yaw][1]  ,f_gimbal_Pid_Para[Yaw][1]);

////	//Kalman Init
////	KalmanCreate(&gyro_Kalman[0],1,20);
////	KalmanCreate(&gyro_Kalman[1],1,20);
//}

///**
//  * @brief          ��̨���ݸ���
//  * @param[out]     Gimbal_Info_update:��̨��Ϣ����ָ��.
//  * @retval         none
//  */
//static void gimbal_info_update(Gimbal_Info_t *Gimbal_Info_update)
//{
//		if(Gimbal_Info_update == NULL) return ;
//		
//		//����������
//		if(rc_ctrl.rc.s[1]==3 || rc_ctrl.rc.s[1]==2){
//				Gimbal_Info_update->Target.pit_angle -= rc_ctrl.rc.ch[1]*0.0002f - rc_ctrl.mouse.y * 0.002f;
//				Gimbal_Info_update->Target.yaw_angle -= rc_ctrl.rc.ch[0]*0.0002f + rc_ctrl.mouse.x * 0.0015f;
//		}
//		else{
//				Gimbal_Info_update->Target.pit_angle = *Gimbal_Info_update->Measure.pit_angle;
//				Gimbal_Info_update->Target.yaw_angle = *Gimbal_Info_update->Measure.yaw_angle;
//		}
//		//�������޷�
//		VAL_Limit(Gimbal_Info_update->Target.pit_angle,-32.5,26);
//}

///**
//  * @brief          ��̨��̬����
//  * @param[out]     Gimbal_Info_control:��̨��Ϣ����ָ��.
//  * @retval         none
//  */
//static void gimbal_posture_ctrl(Gimbal_Info_t *Gimbal_Info_control)
//{
//		if(Gimbal_Info_control == NULL) return ;
//		
//		//�����������ٶ�
//		Gimbal_Info_control->Target.pit_gyro = f_PID_Calculate(&Gimbal_PID[Pitch][0],Gimbal_Info_control->Target.pit_angle,*Gimbal_Info_control->Measure.pit_angle);
//		Gimbal_Info_control->Target.yaw_gyro = f_PID_Calculate(&Gimbal_PID[Yaw][0],  Gimbal_Info_control->Target.yaw_angle,*Gimbal_Info_control->Measure.yaw_angle);
//		
//		//���¿��Ƶ���ֵ
//		Gimbal_Info_control->SendValue[Pitch] = f_PID_Calculate(&Gimbal_PID[Pitch][1],Gimbal_Info_control->Target.pit_gyro,KalmanFilter(&gyro_Kalman[0],*Gimbal_Info_control->Measure.pit_gyro));
//		Gimbal_Info_control->SendValue[Yaw] = f_PID_Calculate(&Gimbal_PID[Yaw][1],  Gimbal_Info_control->Target.yaw_gyro,KalmanFilter(&gyro_Kalman[1],*Gimbal_Info_control->Measure.yaw_gyro));
//		
//		//��̨ж��
//		if(rc_ctrl.rc.s[1]==1 || rc_ctrl.rc.s[1]==0)
//		{
//			Gimbal_Info_control->SendValue[Pitch] = 0;
//			Gimbal_Info_control->SendValue[Yaw] = 0;
//		}
//}

///**
//  * @brief          ��̨���Ƶ�������
//	* @param[out]     Gimbal_Info_send:��̨��Ϣ����ָ��, TXFrame:CAN����֡������ָ��
//  * @retval         none
//  */
//static void chassis_send_current(Gimbal_Info_t *Gimbal_Info_send,CAN_TxFrameTypeDef *TXFrame)
//{
//	if(Gimbal_Info_send == NULL || TXFrame == NULL ) return;
//	
//	TXFrame[0].data[2] = (uint8_t)(Gimbal_Info_send->SendValue[Pitch] >> 8);
//	TXFrame[0].data[3] = (uint8_t)(Gimbal_Info_send->SendValue[Pitch]);
//	TXFrame[0].data[0]   = (uint8_t)(Gimbal_Info_send->SendValue[Yaw] >> 8);
//	TXFrame[0].data[1]   = (uint8_t)(Gimbal_Info_send->SendValue[Yaw]);
//	
//	//CAN_Transmit
//  USER_CAN_TxMessage(&TXFrame[0]);
//}

