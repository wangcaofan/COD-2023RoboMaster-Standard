

#include "cmsis_os.h"

#include "ChassisTask.h"

#include "motor.h"

#include "pid.h"
#include "kalman.h"
#include "INS_Task.h"
#include "ramp.h"
#include "RefereeTask.h"
#include "CANTxTask.h"
float wheel_PID_Param1[6]={13,0.1f,0,0.f,2000.f,15000,};
float wheel_PID_Param2[6]={13,0.1f,0,0.f,2000.f,15000,};
float wheel_PID_Param3[6]={13,0.1f,0,0.f,2000.f,15000,};
float wheel_PID_Param4[6]={13,0.1f,0,0.f,2000.f,15000,};
float wheel_PID_Param5[6]={10.f,0.1f,0,0.f,2000.f,15000,};
PID_Info_TypeDef Chassis_PID[5];
Chassis_Info_t Chassis_Ctrl={
	.working = 0,
  .workMode =0,
	.RB = &Chassis_Motor[Right_B],
  .RF = &Chassis_Motor[Right_F],
  .LF = &Chassis_Motor[Left_F],
	.LB = &Chassis_Motor[Left_B],
	.Yaw = &Chassis_Motor[Yaw],
  .Center_Zeta = 0.f,	
};


float sin_folo,cos_folo;
static void Chassis_init(void);

static void quanxiang_Chassis_Control(Chassis_Info_t *Chassis_Info);
static void Chassis_Info_Update(Gimbal_Message_t *Gimbal_Message,Chassis_Info_t *Chassis_Info);
static void Chassis_power_limit(void);
static void Chasssis_SendValue_Update(Chassis_Info_t *Chassis_Info);
static void vw_get(Chassis_Info_t *Chassis_Info);
static void Get_Buff(Chassis_Info_t *Chassis_Info);
#define POWER_CTRL_ENABLE 1
uint8_t LastMode;
void Chassis_Task(void const * argument)
{
  /* USER CODE BEGIN ShootTask */
	Chassis_init();
	osDelay(1000);
	
  /* Infinite loop */
    for(;;)
	{
	 Chassis_Ctrl.working  = Gimbal2Chassis.chassisWork;
   Chassis_Ctrl.workMode = Gimbal2Chassis.chassisMode;
//	if(Chassis_Ctrl.workMode==1) {
//   Chassis_Ctrl.Center_Zeta = 115.f - Chassis_Motor[Yaw].Data.angle;
//	}else 
		Chassis_Ctrl.Center_Zeta = 136.4f -Chassis_Motor[Yaw].Data.angle;
	if(Chassis_Ctrl.Center_Zeta > 180.f)  Chassis_Ctrl.Center_Zeta -= 360.f;
	if(Chassis_Ctrl.Center_Zeta < -180.f) Chassis_Ctrl.Center_Zeta += 360.f;
     sin_folo=arm_sin_f32(Chassis_Ctrl.Center_Zeta*2*3.1415926535f/360.f);
	   cos_folo=arm_cos_f32(Chassis_Ctrl.Center_Zeta*2*3.1415926535f/360.f);
		  Chassis_Info_Update(&Gimbal2Chassis,&Chassis_Ctrl);
		  vw_get(&Chassis_Ctrl);
	    Get_Buff(&Chassis_Ctrl);
	    quanxiang_Chassis_Control(&Chassis_Ctrl);
	    Chasssis_SendValue_Update(&Chassis_Ctrl);
		  osDelay(1);
	}
}
  /* USER CODE END ShootTask */


static void Chassis_init(void){
	PID_Init(&Chassis_PID[Right_B],PID_POSITION,wheel_PID_Param1);
	PID_Init(&Chassis_PID[Right_F],PID_POSITION,wheel_PID_Param2);
	PID_Init(&Chassis_PID[Left_F] ,PID_POSITION,wheel_PID_Param3);
	PID_Init(&Chassis_PID[Left_B] ,PID_POSITION,wheel_PID_Param4);
	PID_Init(&Chassis_PID[4] ,PID_POSITION,wheel_PID_Param5);
}

static void Chassis_Info_Update(Gimbal_Message_t *Gimbal_Message,Chassis_Info_t *Chassis_Info){
  
	 if(Chassis_Info->working == 1){
		
	   float vy = (Gimbal_Message->vy*cos_folo*21.f)+(-Gimbal_Message->vx*sin_folo *21.f);
	   float vx = (Gimbal_Message->vy*sin_folo*21.f)+ (Gimbal_Message->vx*cos_folo*21.f);
 
		 chassis_ramp(Chassis_Info->workMode,&Chassis_Info->vx,vx,6.f);
     chassis_ramp(Chassis_Info->workMode,&Chassis_Info->vy,vy,6.f);
		 	 if(Chassis_Info->workMode==1){
     VAL_LIMIT(Chassis_Info->vy,-4000,4000);
		 VAL_LIMIT(Chassis_Info->vx,-4000,4000);
		 }
   }
 }

static void vw_get(Chassis_Info_t *Chassis_Info){
	  
	  switch(Chassis_Info->workMode)
    {
      
      case 0:
	       Chassis_Info->vw = 100.f*Chassis_Info->Center_Zeta - 	18.f*(Chassis_Info->Yaw->Data.velocity);
         VAL_LIMIT(Chassis_Info->vw,-5200,5200);
			break;
      case 1:
	      Chassis_Info->vw =-(6000.f + robot_state.robot_level*500.f);
			  VAL_LIMIT(Chassis_Info->vw,-10000,10000);
			break;
			case 2:
	     Chassis_Info->vw = 200.f*(Chassis_Info->Center_Zeta + 45.0f)
               - 18.f*(Chassis_Info->Yaw->Data.velocity);
            VAL_LIMIT(Chassis_Info->vw,-2800,2800);
      break;
    default:
			  break;
   }
	
}
float Buff=1.f,Sup_buff =1.f,Exp_buff = 1.f;
static void Get_Buff(Chassis_Info_t *Chassis_Info){

	   Exp_buff =0.9f+robot_state.robot_level*0.1;
	   if(Chassis_Info->workMode ==0 && SuperCap_Info.DischargeATK==0 && SuperCap_Info.Cap_low_waring ==0){
				 Sup_buff =1.6;
			   Exp_buff =1.3;
		  }else {
			   Sup_buff =1;
			   Exp_buff =1;
			}
	  
		 if(SuperCap_Info.DischargeATK==0&&Chassis_Info->workMode==1){
			  Exp_buff =1.3;
		  if (SuperCap_Info.CapQuantity>=90) 	
		  {
				Sup_buff =1.6;
			}
			else if(SuperCap_Info.CapQuantity>=75)
			{
				Sup_buff =1.4;
			}
			else if(SuperCap_Info.CapQuantity>=55)
			{
				Sup_buff =1.2;
			} else {Sup_buff =1.0; Exp_buff=1,0;}
		 }
     Buff =  Sup_buff*Exp_buff;
		 VAL_LIMIT(Buff,1.0f,2.0f);
}

static void Chassis_power_limit(void){
	 double Scaling[4] = {0.f};
   double Chassis_PidOut = 0.f;
   float  KLimit = 0.f,PLimit = 1.f;
	
   Chassis_PidOut =   fabs(Chassis_PID[0].Err[0])
	                  + fabs(Chassis_PID[1].Err[0])
	                  + fabs(Chassis_PID[2].Err[0])
	                  + fabs(Chassis_PID[3].Err[0]);
   for(int i=0;i<4;i++){
	   Scaling[i] = (Chassis_PID[i].Err[0]) / (Chassis_PidOut ? Chassis_PidOut:1);
	 }
   KLimit =  Chassis_PidOut/36000;
   VAL_LIMIT(KLimit,-1,1);
   if(power_heat_data_t.chassis_power_buffer==60 ) PLimit = 1.0;
   else if(power_heat_data_t.chassis_power_buffer >=45 ) PLimit = 0.85;
   else if(power_heat_data_t.chassis_power_buffer >=40 ) PLimit = 0.7;
	 else if(power_heat_data_t.chassis_power_buffer >=35 ) PLimit = 0.55;
	 else if(power_heat_data_t.chassis_power_buffer >=30 ) PLimit = 0.4;
	 else if(power_heat_data_t.chassis_power_buffer >=20 ) PLimit = 0.25;
   else if(power_heat_data_t.chassis_power_buffer >=10 ) PLimit = 0.1;
   else if(power_heat_data_t.chassis_power_buffer <10)   PLimit = 0.05  ;
	 for(int i=0;i<4;i++)
	{
  	Chassis_PID[i].Output = 61536.f * Scaling[i] * KLimit * PLimit*Buff;                                                                                                                                                                                                                                
	}
}
static void quanxiang_Chassis_Control(Chassis_Info_t *Chassis_Info )
{  
	static float vx_com,vy_com,vw_com;
	if(Chassis_Ctrl.workMode==1&&(Gimbal2Chassis.vx!=0 || Gimbal2Chassis.vy!=0 )) {
		vy_com = (-Chassis_Motor[Right_B].Data.velocity - Chassis_Motor[Right_F].Data.velocity + Chassis_Motor[Left_F].Data.velocity + Chassis_Motor[Left_B].Data.velocity)/4.8f;
	  vx_com=  (-Chassis_Motor[Right_B].Data.velocity + Chassis_Motor[Right_F].Data.velocity - Chassis_Motor[Left_F].Data.velocity + Chassis_Motor[Left_B].Data.velocity)/4.8f;
	  vw_com = ( Chassis_Motor[Right_B].Data.velocity + Chassis_Motor[Right_F].Data.velocity + Chassis_Motor[Left_F].Data.velocity + Chassis_Motor[Left_B].Data.velocity)/4.f;
	}else {
	   vy_com=0;
	   vx_com=0;
	   vw_com=0;
	}
  Chassis_Info->Target.RB_velocity = ( - (Chassis_Info->vy-vy_com) -  (Chassis_Info->vx-vx_com)  + Chassis_Info->vw-vw_com);
  Chassis_Info->Target.RF_velocity = ( - (Chassis_Info->vy-vy_com) +  (Chassis_Info->vx-vx_com)  + Chassis_Info->vw-vw_com );
  Chassis_Info->Target.LF_velocity = (   (Chassis_Info->vy-vy_com) +  (Chassis_Info->vx-vx_com)  + Chassis_Info->vw-vw_com );
  Chassis_Info->Target.LB_velocity = (   (Chassis_Info->vy-vy_com) -  (Chassis_Info->vx-vx_com)  + Chassis_Info->vw-vw_com);

   f_PID_Calculate(&Chassis_PID[Right_B],Chassis_Info->Target.RB_velocity,Chassis_Ctrl.RB->Data.velocity);
	 f_PID_Calculate(&Chassis_PID[Right_F],Chassis_Info->Target.RF_velocity,Chassis_Ctrl.RF->Data.velocity);
   f_PID_Calculate(&Chassis_PID[Left_F], Chassis_Info->Target.LF_velocity,Chassis_Ctrl.LF->Data.velocity);
	 f_PID_Calculate(&Chassis_PID[Left_B], Chassis_Info->Target.LB_velocity,Chassis_Ctrl.LB->Data.velocity);
	 #if POWER_CTRL_ENABLE
	   Chassis_power_limit();
   #endif
}

static void Chasssis_SendValue_Update(Chassis_Info_t *Chassis_Info){
	    if(Chassis_Info->working ==1){
      Chassis_Info->SendValue[Right_B] =Chassis_PID[Right_B].Output;
      Chassis_Info->SendValue[Right_F] =Chassis_PID[Right_F].Output;
	    Chassis_Info->SendValue[Left_F] =Chassis_PID[Left_F].Output;
	    Chassis_Info->SendValue[Left_B] =Chassis_PID[Left_B].Output;
      }else {
		  Chassis_Info->SendValue[Right_B] =0;
      Chassis_Info->SendValue[Right_F] =0;
	    Chassis_Info->SendValue[Left_F]  =0;
	    Chassis_Info->SendValue[Left_B]  =0;
		}
}
  



