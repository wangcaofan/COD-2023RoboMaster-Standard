

#include "cmsis_os.h"

#include "ControlTask.h"

#include "motor.h"
#include "tim.h"
#include "pid.h"
#include "kalman.h"
#include "INS_Task.h"
#include "Vision_Task.h"
#include "api_trajectory.h"
#include "ramp.h"
#define Shoot_Heat Chassis_Message.shoot_remainingHeat
int8_t stuck_flag = 1;//卡弹时变号反转
Gimbal_Info_t gimbal_Ctrl={
		.rc_ctrl = &rc_ctrl,
		.Measure.pit_angle = &INS_Info.pit_angle,
		.Measure.yaw_angle = &INS_Info.yaw_angle,
		.Measure.pit_gyro  = &INS_Info.pit_gyro,
		.Measure.yaw_gyro  = &INS_Info.yaw_gyro,
	  .Target.pit_angle =0,
	  .Target.yaw_angle =0,
		.Limit_pitch.max = -32.5f,
		.Limit_pitch.min = 26.f,
	  .Working =1,
};
Shoot_Info_t Shoot_Ctrl = {
	  .CoverSwitch = 0,
    .trigger_Buf=8,//拨盘叶数
	  .FrictionWheelSpeed=
    {
        [_0M]=0,
        [_15M]=4450,
        [_18M]=4900,
	    	[_22M]=5500,
        [_30M]=7100,
    },
		
    .wheel_L=&Gimbal_Motor[SHOOTL],
    .wheel_R=&Gimbal_Motor[SHOOTR],
    .trigger=&Gimbal_Motor[TRIGGER],
    .zibao=0,
};

Send2Chassis_Info_t Send2chassis_Message={
  .SupercapWork = 0,
  .ChassisWork = 0,
  .ChassisMode = 0,
};
Chassis_Message_t Chassis_Message ={
   .shoot_remainingHeat=0,
   .shoot_speedGain=0,
   .shoot_expectSpeed=0,
   
};
PID_Info_TypeDef Gimbal_PID[2][2],Gipmbal_Auto_PID[2][2],ShootSpeed_PID[2],Trigger_PID[2];

float f_gimbal_Pid_Para[2][2][6]={
	[0]={
		[0]={25.f,0.f,0.f,0.f,0.f,30000,},
		[1]={100.f,0.1f,0.f,0.f,10000.f,30000,},
	},
	[1]={	
		[0]={23.f,0.0f,0.f,0.f,0.f,30000,},
		[1]={100.f,0.15f,0,0.f,15000.f,30000,},/*抖*/
	},
};
//float f_gimbal_auto_Pid_Para[2][2][6]={
//	[0]={
//		[0]={80.f,0.f,0.f,0.f,0.f,30000,},
//		[1]={100.f,0.4f,0.f,0.f,10000.f,30000,},
//	},
//	[1]={	
//		[0]={23.f,0.0f,0.f,0.f,0.f,30000,},
//		[1]={100.f,0.2f,0,0.f,3000.f,30000,},/*抖*/
//	},
//};
float wheel_PID_Param[6]={13,0.1f,0,0.f,1000.f,15000,};
float trigger_PID_Param[2][6]={
			[0]={18,0.1f,0,0,1000.f,15000,},
			[1]={16,0.f,0,0,0,10000,},
};


static void gimbal_init(void);
static void Gimbal_Control(Gimbal_Info_t *Gimbal_Info, Shoot_Info_t *Shoot_Info,Send2Chassis_Info_t *Send2Chassis_Info);
static void Shoot_Control(Gimbal_Info_t *Gimbal_Info, Shoot_Info_t *Shoot_Info,Send2Chassis_Info_t *Send2Chassis_Info);
void fire_ctrl(void);
/* USER CODE BEGIN Header_ShootTask */
/**
* @brief Function implementing the myShootTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ShootTask */
void Control_Task(void const * argument)
{
  /* USER CODE BEGIN ShootTask */

  osDelay(1000);
	gimbal_init();
	TickType_t systick = 0;
	
  /* Infinite loop */
  for(;;){
	systick = xTaskGetTickCount();
						Gimbal_Control(&gimbal_Ctrl,&Shoot_Ctrl,&Send2chassis_Message);
				    Shoot_Control(&gimbal_Ctrl,&Shoot_Ctrl,&Send2chassis_Message);
	          osDelayUntil(&systick,1);
			      }
	         
	  }
   	
  
  /* USER CODE END ShootTask */


static void gimbal_init(void){
	PID_Init(&Gimbal_PID[Pitch][0],PID_POSITION,f_gimbal_Pid_Para[Pitch][0]);
	PID_Init(&Gimbal_PID[Pitch][1],PID_POSITION,f_gimbal_Pid_Para[Pitch][1]);
	PID_Init(&Gimbal_PID[Yaw][0] ,PID_POSITION,f_gimbal_Pid_Para[Yaw][0]);
	PID_Init(&Gimbal_PID[Yaw][1] ,PID_POSITION,f_gimbal_Pid_Para[Yaw][1]);
	
//	PID_Init(&Gimbal_Auto_PID[Pitch][0],PID_POSITION,f_gimbal_Pid_Para[Pitch][0]);
//	PID_Init(&Gimbal_Auto_PID[Pitch][1],PID_POSITION,f_gimbal_Pid_Para[Pitch][1]);
//	PID_Init(&Gimbal_Auto_PID[Yaw][0] ,PID_POSITION,f_gimbal_Pid_Para[Yaw][0]);
//	PID_Init(&Gimbal_Auto_PID[Yaw][1] ,PID_POSITION,f_gimbal_Pid_Para[Yaw][1]);
  
	PID_Init(&ShootSpeed_PID[0],PID_POSITION,wheel_PID_Param);
	PID_Init(&ShootSpeed_PID[1],PID_POSITION,wheel_PID_Param);
	PID_Init(&Trigger_PID[0],PID_POSITION,trigger_PID_Param[0]);
	PID_Init(&Trigger_PID[1],PID_POSITION,trigger_PID_Param[1]);
  

}



float ramp_fuction(float target,float input,float ramp)
{
  float error  = target - input;
  float output = input;

	if (error > 0)
	{
        if (error > ramp){output += ramp;}   
        else{output += error;}
    }else
	{
        if (error < -ramp){output += -ramp;}
        else{output += error;}
    }
    return output;
}


/**
  * @brief          云台姿态控制
  * @param[out]     Gimbal_Info_control:云台信息变量指针.
  * @retval         none
  */
float k_yaw=0;
float Yaw_Angle_Err,a;
float Yaw_Speed_Err;
float Yaw_Targer_Angle_Err;
float pit_Targer_Angle_Err;
float Pit_Angle_Err,pit_buff,Yawqiankui,yaw_buff;
float test[7]={0};
static void Gimbal_Control(Gimbal_Info_t *Gimbal_Info, Shoot_Info_t *Shoot_Info,Send2Chassis_Info_t *Send2Chassis_Info)
{
	 
		if(Gimbal_Info->Working == 1 && rc_ctrl.online_Cnt>50){
			

			

	     if(rc_ctrl.rc.s[1]==1&&rc_ctrl.rc.s[0]==2){
			   Send2Chassis_Info -> vy =0;
			   Send2Chassis_Info -> vx =0;
			 }else{
			 Send2Chassis_Info -> vy = rc_ctrl.rc.ch[3]
                            + (rc_ctrl.key.set.W-rc_ctrl.key.set.S)*660;
	
	     Send2Chassis_Info -> vx = rc_ctrl.rc.ch[2] 
                            + (rc_ctrl.key.set.D-rc_ctrl.key.set.A)*660;
			 }
       VAL_LIMIT(Gimbal_Info ->Target.pit_angle,-25,33);
		  
	   
		 if(Shoot_Info ->CoverSwitch) Gimbal_Info ->Target.pit_angle = -10.f;
		
		   Gimbal_Info ->Target.pit_angle += rc_ctrl.rc.ch[1]*0.00015f  - rc_ctrl.mouse.y * 0.0017f;
		   Gimbal_Info ->Target.yaw_angle -= rc_ctrl.rc.ch[0]*0.00023f  + rc_ctrl.mouse.x * 0.0015f;



if(rc_ctrl.mouse.press_r==1&&Vision_Info.IF_Fire_Accept==1){

	       Gimbal_Info ->Target.pit_angle =Vision_Info.target_Pitch;
				 Gimbal_Info ->Target.yaw_angle = Vision_Info.target_Yaw;
			  
     }
		
 
	  if( Gimbal_Info ->Target.yaw_angle > 180)  Gimbal_Info ->Target.yaw_angle -=360;
		if( Gimbal_Info ->Target.yaw_angle <-180)  Gimbal_Info ->Target.yaw_angle +=360;		
		VAL_LIMIT(Gimbal_Info ->Target.pit_angle,-25,33);
		

		 
		 Pit_Angle_Err	=  Gimbal_Info ->Target.pit_angle +2.1*(Send2Chassis_Info->ChassisMode==1)- INS_Info.pit_angle;
		 Yaw_Angle_Err  =  Gimbal_Info ->Target.yaw_angle - INS_Info.yaw_angle;	
		
		if(Yaw_Angle_Err > 180.f )   Yaw_Angle_Err -= 360.f;
	  if(Yaw_Angle_Err < -180.f)   Yaw_Angle_Err += 360.f;
   	Yaw_Targer_Angle_Err=Gimbal_Info->Target.yaw_angle - Gimbal_Info->Last_Target.yaw_angle;
		Gimbal_Info->Last_Target.yaw_angle =Gimbal_Info->Target.yaw_angle;
 	  pit_Targer_Angle_Err=Gimbal_Info->Target.pit_angle - Gimbal_Info->Last_Target.pit_angle;
		Gimbal_Info->Last_Target.pit_angle =Gimbal_Info->Target.pit_angle;
//		if(Send2Chassis_Info->ChassisMode==1&&Send2Chassis_Info->vx==0&&Send2Chassis_Info->vy==0)   k_yaw=0.4; 
//		else  k_yaw=0;
		
		Gimbal_Info->Target.pit_gyro = f_PID_Calculate(&Gimbal_PID[Pitch][0],Pit_Angle_Err,0)+pit_Targer_Angle_Err*2;
		Gimbal_Info->Target.yaw_gyro = f_PID_Calculate(&Gimbal_PID[Yaw][0], Yaw_Angle_Err,0)+Yaw_Targer_Angle_Err*3; //-INS_Info.yaw_gyro *;
	  
		Gimbal_Info->SendValue[Pitch] = f_PID_Calculate(&Gimbal_PID[Pitch][1],Gimbal_Info->Target.pit_gyro,INS_Info.pit_gyro);
		Gimbal_Info->SendValue[Yaw]   = f_PID_Calculate(&Gimbal_PID[Yaw][1],  Gimbal_Info->Target.yaw_gyro,INS_Info.yaw_gyro);
	  } else if (Gimbal_Info->Working==0)
		{
				Gimbal_Info ->Target.pit_angle =INS_Info.pit_angle;
				Gimbal_Info ->Target.yaw_angle = INS_Info.yaw_tolangle ;
        Send2Chassis_Info->ChassisWork = 0 ;
		    Shoot_Info->Working =0; 
				Gimbal_Info->SendValue[Pitch] = 0 ;
		    Gimbal_Info->SendValue[Yaw] =0 ;
    }
}

static void Shoot_Control(Gimbal_Info_t *Gimbal_Info, Shoot_Info_t *Shoot_Info,Send2Chassis_Info_t *Send2Chassis_Info)
{
   if(Shoot_Info->Working ==1 ){
		 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
		 
		 fire_ctrl();
     
		 switch (Chassis_Message.shoot_expectSpeed)
		 {
			 case 15 :
		    Shoot_Info->Target.Shoot_velocity = Shoot_Info->FrictionWheelSpeed[_15M] ;
		    break;
		   case 18 :
		    Shoot_Info->Target.Shoot_velocity = Shoot_Info->FrictionWheelSpeed[_18M] ;
		    break;
			 case 22 :
		    Shoot_Info->Target.Shoot_velocity = Shoot_Info->FrictionWheelSpeed[_22M] ;
		    break;
		   case 30 :
		    Shoot_Info->Target.Shoot_velocity = Shoot_Info->FrictionWheelSpeed[_30M] ;
		    break;
			 default:
  			  break;
		 }
   
	}else{
	    Shoot_Info->Target.Shoot_velocity = Shoot_Info->FrictionWheelSpeed[_0M];
	    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);

	  }
     if(Shoot_Info->CoverSwitch==1){
	    __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,500);
	   }else __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,2160);
     Shoot_Info->SendValue[SHOOTL] = f_PID_Calculate(&ShootSpeed_PID[0],-Shoot_Info->Target.Shoot_velocity,Shoot_Info->wheel_L->Data.velocity);
		 Shoot_Info->SendValue[SHOOTR] = f_PID_Calculate(&ShootSpeed_PID[1],Shoot_Info->Target.Shoot_velocity,Shoot_Info->wheel_R->Data.velocity);
}
 
void fire_ctrl(void){
	  float Angle_Err,Velocity_Err;
    static uint8_t Flag_Mouse_Left;
	  static int Fire_Flag =1;
	  float Fire_Gain = 1; 
if(rc_ctrl.mouse.press_r==1)  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
	 if( Shoot_Heat > 15) Fire_Flag = 1; else Fire_Flag =0;
	      if (Shoot_Heat > 120 ) Fire_Gain =1.2f;	
	 else if (Shoot_Heat > 90  ) Fire_Gain =0.9f;
	 else if (Shoot_Heat> 60 )	 Fire_Gain =0.6f;
	 else if (Shoot_Heat> 30 )	 Fire_Gain =0.3f;
	 else 											 Fire_Gain =0.1f;
   
	 if( Shoot_Ctrl.Workmode ==Single){
	    if(rc_ctrl.mouse.press_l == 0 && rc_ctrl.rc.ch[3] <=630 ) Flag_Mouse_Left=0;
	    if(Flag_Mouse_Left==0 &&(rc_ctrl.mouse.press_l==1||rc_ctrl.rc.ch[3]==660)){
        Flag_Mouse_Left =1;
				if(Fire_Flag==1){
			   Shoot_Ctrl.Target.Trigger_angle = Gimbal_Motor[TRIGGER].Data.angle - 360.f/Shoot_Ctrl.trigger_Buf;
	      }
			  if(Shoot_Ctrl.Target.Trigger_angle <=0) Shoot_Ctrl.Target.Trigger_angle += 360;
		    }
			  
	      Angle_Err =  Shoot_Ctrl.Target.Trigger_angle - Gimbal_Motor[4].Data.angle;
				
        if(Angle_Err<-180.0f)Angle_Err+=360.0f;
        if(Angle_Err>=180.0f)Angle_Err-=360.0f;
		    Velocity_Err = Angle_Err*57.29578f - Gimbal_Motor[4].Data.velocity;
	  }
   if(Shoot_Ctrl.Workmode ==Consecutive){//连发
		   Velocity_Err = -rc_ctrl.mouse.press_l *5000 *Fire_Gain -  Gimbal_Motor[4].Data.velocity;
		   if(rc_ctrl.rc.ch[3]>500)  {Velocity_Err -= (rc_ctrl.rc.ch[3]*8 *Fire_Gain);}
	   }
	 if(Shoot_Ctrl.zibao==1) {
		    Velocity_Err = -rc_ctrl.mouse.press_l *6000  -  Gimbal_Motor[4].Data.velocity;
		  }
		Shoot_Ctrl.SendValue[TRIGGER]      = f_PID_Calculate(&Trigger_PID[0],Velocity_Err,0);
}
