//
// Created by YanYuanbin on 22-10-5.
//

#include "cmsis_os.h"

#include "StateTask.h"
#include "INS_Task.h"


#include "remote_control.h"
#include "bsp_can.h"
#include "tim.h"
#include "ControlTask.h"


/* Private function prototypes -----------------------------------------------*/
static void rc_ctrl_monitor(rc_ctrl_t *rc_ctrl);
static void RC_Control(Gimbal_Info_t *Gimbal_Info , Shoot_Info_t *Shoot_Info,Send2Chassis_Info_t *Send2Chassis_Info);
static void KeyBoard_Control(Gimbal_Info_t *Gimbal_Info , Shoot_Info_t *Shoot_Info,Send2Chassis_Info_t *Send2Chassis_Info);
  uint8_t rc_s_flag[4][4]={0};
//static void rc_communicate(rc_ctrl_t *rc_ctrl,CAN_TxFrameTypeDef *TxHeader);

/* USER CODE BEGIN Header_StateTask */
/**
* @brief Function implementing the myStateTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StateTask */
	TickType_t systick = 0;
void State_Task(void const * argument)
{
  /* USER CODE BEGIN SerialTask */
	
   	osDelay(1000);
  /* Infinite loop */
  for(;;)
  {
	systick = xTaskGetTickCount();
	if(rc_ctrl.online_Cnt < 50){
		   gimbal_Ctrl.Working =0;
		   Shoot_Ctrl.Working =0;
		   Shoot_Ctrl.CoverSwitch =0;
		   Send2chassis_Message.ChassisWork=0;
			 Send2chassis_Message.ChassisMode=0;
			 for(int i=0;i<4;i++){
					for(int j=0;j<4;j++){
						rc_s_flag[i][j] = 0;
					}
				}
	}else if(rc_ctrl.online_Cnt > 50){
	   if(rc_ctrl.rc.s[1]==1&&rc_ctrl.rc.s[0]==1) KeyBoard_Control(&gimbal_Ctrl,&Shoot_Ctrl,&Send2chassis_Message);
		 else RC_Control(&gimbal_Ctrl,&Shoot_Ctrl,&Send2chassis_Message);
	 }
 	 rc_ctrl_monitor(&rc_ctrl);
   osDelayUntil(&systick,5);
	}
}

 void RC_Control(Gimbal_Info_t *Gimbal_Info , Shoot_Info_t *Shoot_Info,Send2Chassis_Info_t *Send2Chassis_Info)
{     
	       Gimbal_Info->Working = 1;
			   Send2Chassis_Info->ChassisWork=1;
	     
        switch(rc_ctrl.rc.s[1]){
					case 1:
					  switch (rc_ctrl.rc.s[0]){
								case 2:
                 if(rc_s_flag[1][2]==0){
					        for(int i=0;i<4;i++){
							       for(int j=0;j<4;j++){
								       rc_s_flag[i][j] = 0;
							         }
						        }
										rc_s_flag[1][2]=1;
					        Shoot_Info->Working = 1;
									Shoot_Ctrl.Workmode=Consecutive;
		              Shoot_Info->CoverSwitch =0;
                  Send2Chassis_Info->ChassisMode=0;
									}break;
							default:
                break;
							 }
						break;
				  case 3:
						  switch(rc_ctrl.rc.s[0]){
								case 2:
							  if(rc_s_flag[3][2]==0){
					        for(int i=0;i<4;i++){
							       for(int j=0;j<4;j++){
								       rc_s_flag[i][j] = 0;
							         }
						        }
                  rc_s_flag[3][2]= 1;									
									Shoot_Info->Working = 0;
		              Shoot_Info->CoverSwitch = 1;
			            Send2Chassis_Info->ChassisMode=0;
					       }break;
					    	default:
								 break;
						 }
							break;
					case 2:
					    switch (rc_ctrl.rc.s[0]){
							  case 2:
							    if(rc_s_flag[2][2] == 0){
						        for(int i=0;i<4;i++){
							        for(int j=0;j<4;j++){
								       rc_s_flag[i][j] = 0;
							         }
						          }
						      rc_s_flag[2][2] = 1;
		              Shoot_Info->Working = 0;
		              Shoot_Info->CoverSwitch =0;
									Send2Chassis_Info->ChassisMode=0;
		           }break;
								case 3:
							    if(rc_s_flag[2][3] == 0){
						        for(int i=0;i<4;i++){
							        for(int j=0;j<4;j++){
								       rc_s_flag[i][j] = 0;
							         }
						          }
						      rc_s_flag[2][3] = 1;
		              Shoot_Info->Working =0;
		              Shoot_Info->CoverSwitch =0;
			            Send2Chassis_Info->ChassisMode=1;
		           }break;
					      case 1:
							    if(rc_s_flag[2][1] == 0){
						        for(int i=0;i<4;i++){
							        for(int j=0;j<4;j++){
								       rc_s_flag[i][j] = 0;
							         }
						          }
						      rc_s_flag[2][1] = 1;
		              Shoot_Info->Working =0;
		              Shoot_Info->CoverSwitch =0;
			            Send2Chassis_Info->ChassisMode=0;
		           }break;
					      
								default:
                 break;
							}
							break;
				 	default:
           break;	 
      }
				
   
}
			

static void KeyBoard_Control(Gimbal_Info_t *Gimbal_Info , Shoot_Info_t *Shoot_Info,Send2Chassis_Info_t *Send2Chassis_Info){
     Gimbal_Info->Working = 1;
		 Send2Chassis_Info->ChassisWork=1;
		 Gimbal_Info->Workmode = Consecutive ;
	   Shoot_Info->Working=1;
	Send2Chassis_Info->ChassisMode = 0;
	   static uint8_t Flag_R = 0;
		 if(rc_ctrl.key.set.R == 1 && Flag_R==0){
		    Shoot_Info->CoverSwitch = !Shoot_Info->CoverSwitch;
			  Flag_R = 1;
		 }else if(rc_ctrl.key.set.R==0){
		    Flag_R = 0;
		 }
		 static uint8_t Flag_Q = 0;
		 if(rc_ctrl.key.set.Q == 1 && Flag_Q==0){
		    Shoot_Info -> Workmode = ! Shoot_Info -> Workmode;
			  Flag_Q = 1;
		 }else if(rc_ctrl.key.set.Q==0){
		    Flag_Q = 0;
		 }
//		 static uint8_t Flag_F = 0;
//		 if(rc_ctrl.key.set.F == 1 && Flag_F==0){
//		   Shoot_Info->FrictionWheelSpeed[BULLET_SPEED_NUM]-=30;
//			  Flag_F = 1;
//		 }else if(rc_ctrl.key.set.F==0){
//		    Flag_F = 0;
//		 }
		 static uint8_t Flag_E = 0,Gyroscope =0;
		 if(rc_ctrl.key.set.E == 1 && Flag_E==0){
		    Gyroscope = !Gyroscope;
			  Flag_E = 1;
		 }else if(rc_ctrl.key.set.E==0){
		    Flag_E = 0;
		 }
		 if(rc_ctrl.key.set.SHIFT==1) Send2Chassis_Info->ChassisMode = 1;
		 if(Gyroscope == 1) Send2Chassis_Info->ChassisMode = 1;
		  if(rc_ctrl.key.set.CTRL==1)  Send2Chassis_Info->ChassisMode=2;
		
     static uint8_t Flag_F = 0;
		 static uint32_t SupercapSwitch;
		 if(rc_ctrl.key.set.F == 1 && Flag_F == 0){
			 SupercapSwitch = xTaskGetTickCount();
		   Send2Chassis_Info->SupercapWork =  ! Send2Chassis_Info->SupercapWork;
			  Flag_F = 1;
		 }else if(rc_ctrl.key.set.F==0){
		    Flag_F = 0;
		 }
		 if(xTaskGetTickCount()-SupercapSwitch>10000&&Send2Chassis_Info->SupercapWork==1) {
			 Send2Chassis_Info->SupercapWork =0;
       SupercapSwitch=0;
     }			 
		   static uint8_t Flag_G = 0;
		 static uint32_t ZibaoSwitch;
		 if(rc_ctrl.key.set.G == 1 && Flag_G==0){
			 			 ZibaoSwitch = xTaskGetTickCount();
		   Shoot_Info->zibao = !  Shoot_Info->zibao;
			  Flag_G =1;
		 }else if(rc_ctrl.key.set.G==0){
		    Flag_G = 0;
		 }
		 if(xTaskGetTickCount()-ZibaoSwitch>20000&& Shoot_Info->zibao==1) {
			 Shoot_Info->zibao=0;
       ZibaoSwitch=0;
     }
	 
  }


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
