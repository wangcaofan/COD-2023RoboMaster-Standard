#include "cmsis_os.h"

#include "INS_Task.h"
#include "Chassistask.h"
#include "RefereeTask.h"

#include "bsp_can.h"
#include "motor.h"
#include "CANTxTask.h"
#include "pid.h"
#include "kalman.h"
Gimbal_Message_t Gimbal2Chassis={
 .vx=0,.vy=0,

};
SuperCap_Info_t SuperCap_Info={
 .SuperCapSwitch =0,
};
 Send2SuperCap_Info_t Send2SuperCap_Info;
void CANTx_Task(void const * argument)
{
	osDelay(1000);
  /* USER CODE BEGIN ChassisTask */
	
	for(;;)
  {    

		
  ChassisTxFrame.data[0] = (uint8_t)(Chassis_Ctrl.SendValue[0] >> 8);
	ChassisTxFrame.data[1] = (uint8_t)(Chassis_Ctrl.SendValue[0]);
	ChassisTxFrame.data[2] = (uint8_t)(Chassis_Ctrl.SendValue[1] >> 8);
	ChassisTxFrame.data[3] = (uint8_t)(Chassis_Ctrl.SendValue[1]);
	ChassisTxFrame.data[4] = (uint8_t)(Chassis_Ctrl.SendValue[2] >> 8);
	ChassisTxFrame.data[5] = (uint8_t)(Chassis_Ctrl.SendValue[2]);
	ChassisTxFrame.data[6] = (uint8_t)(Chassis_Ctrl.SendValue[3] >> 8);
	ChassisTxFrame.data[7] = (uint8_t)(Chassis_Ctrl.SendValue[3]);
	USER_CAN_TxMessage(&ChassisTxFrame);


	Send2GimbalTxFrame.data[0] =  (uint8_t)(Referee_Message.shoot_remainingHeat>>8);
  Send2GimbalTxFrame.data[1] =  (uint8_t)(Referee_Message.shoot_remainingHeat);		
	Send2GimbalTxFrame.data[2] =  (uint8_t)(Referee_Message.shoot_speedGain>>8);
  Send2GimbalTxFrame.data[3] =  (uint8_t)(Referee_Message.shoot_speedGain);
  Send2GimbalTxFrame.data[4] =  (uint8_t)(Referee_Message.shoot_expectSpeed>>8);
 	Send2GimbalTxFrame.data[5] =  (uint8_t)(Referee_Message.shoot_expectSpeed);
  Send2GimbalTxFrame.data[6] =  ((int)(INS_Info.yaw_gyro*100)>>8);
	Send2GimbalTxFrame.data[7] =  ((int)(INS_Info.yaw_gyro*100));
  USER_CAN_TxMessage(&Send2GimbalTxFrame);   
	
  Send2SuperCap_Info.Power_buffer  = (uint16_t)power_heat_data_t.chassis_power_buffer;
	Send2SuperCap_Info.Power_limit   = (uint16_t)robot_state.chassis_power_limit;  
   
  SupercapTxFrame.data[0] = (uint8_t)(Gimbal2Chassis.SupercapWork);
	SupercapTxFrame.data[1] = 0;
	SupercapTxFrame.data[2] = (uint8_t)(Send2SuperCap_Info.Power_buffer>>8);
	SupercapTxFrame.data[3] = (uint8_t)(Send2SuperCap_Info.Power_buffer);
	SupercapTxFrame.data[4] = 0;
	SupercapTxFrame.data[5] = 0;
	SupercapTxFrame.data[6] = (uint8_t)(Send2SuperCap_Info.Power_limit>>8);
	SupercapTxFrame.data[7] = (uint8_t)(Send2SuperCap_Info.Power_limit);
	USER_CAN_TxMessage(&SupercapTxFrame); 
	osDelay(2);
  }
  /* USER CODE END ChassisTask */
}




