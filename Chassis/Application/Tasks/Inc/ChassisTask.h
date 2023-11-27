
#ifndef _SHOOT_TASK_H
#define _SHOOT_TASK_H

#include "stdint.h"
#include "motor.h"
#include "remote_control.h"
#include "bsp_can.h"

typedef struct
{
  
  uint8_t working;

  uint8_t workMode;
  
 
  float Center_Zeta;

  float vx,vy,vw;

	struct{
	   int16_t RB_velocity;
		 int16_t RF_velocity;
	   int16_t LF_velocity;
		 int16_t LB_velocity;
	 }Target;
 
  DJI_MOTOR *RB,*RF,*LF,*LB,*Yaw;
  int16_t SendValue[8];
}Chassis_Info_t ;


extern Chassis_Info_t Chassis_Ctrl;

extern PID_Info_TypeDef Chassis_PID[5];



#endif


