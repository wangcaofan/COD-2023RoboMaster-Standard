
#ifndef _SHOOT_TASK_H
#define _SHOOT_TASK_H

#include "stdint.h"
#include "motor.h"
#include "remote_control.h"



#define SHOOT_SPEED_15M_S    4600
#define SHOOT_SPEED_18M_S    5100
#define SHOOT_SPEED_30M_S    7450

#define TRIGGER_FREQ_2_HZ     750
#define TRIGGER_FREQ_3_HZ     1000
#define TRIGGER_FREQ_4_HZ     1250
#define TRIGGER_FREQ_5_HZ     1500
#define TRIGGER_FREQ_6_HZ     1750
#define TRIGGER_FREQ_7_HZ     2000
#define TRIGGER_FREQ_8_HZ     2400
#define TRIGGER_FREQ_9_HZ     2700
#define TRIGGER_FREQ_10_HZ    3000


typedef enum{
	  Consecutive,
    Single,
	  Shoot_Mode_Num,
}Shoot_Mode;
typedef enum
{
    _0M,
    _15M,
    _18M,
	  _22M,
    _30M,
   BULLET_SPEED_NUM,
}BULLET_SPEED;

typedef struct 
{
	  uint8_t CoverSwitch;
	  uint8_t Working;
    Shoot_Mode Workmode;
    uint8_t trigger_Buf;
    
	float wheel_Speed;
    float trigger_Angle;
  struct{
		int16_t Shoot_velocity;
	  int16_t Trigger_velocity;
		float Trigger_angle;
	 }Target;
	   uint8_t zibao;
	  int16_t SendValue[6];
	  int16_t FrictionWheelSpeed[BULLET_SPEED_NUM];
    DJI_MOTOR *wheel_L,*wheel_R,*trigger;
       
}Shoot_Info_t;
extern Shoot_Info_t shoot;

typedef struct _Gimbal_Info
{
	//遥控信息
	rc_ctrl_t *rc_ctrl;
	
	//云台状态期望值
	struct{
		float pit_angle;
		float yaw_angle;
		float pit_gyro;
		float yaw_gyro;
	}Target;
	
	struct{
		float pit_angle;
		float yaw_angle;
		float pit_gyro;
		float yaw_gyro;
	}Last_Target;
	//云台状态观测量
	struct{
		float *pit_angle;
		float *yaw_angle;
		float *pit_gyro;
		float *yaw_gyro;
	}Measure;
	
	//pitch角限幅
	struct{
		float min;
		float max;
	}Limit_pitch;
	
	//控制电流发送
	int16_t SendValue[2];
  uint8_t Working;
	uint8_t Workmode;
}Gimbal_Info_t;
typedef struct{
	   
      uint8_t AllState;
    
      uint8_t SupercapWork ;
    
      uint8_t ChassisWork;
     
      uint8_t ChassisMode;
    
      int16_t vx,vy;
}Send2Chassis_Info_t;
typedef  struct{
      int16_t shoot_remainingHeat;
      int16_t shoot_speedGain;
      uint16_t shoot_expectSpeed;
	    int16_t Chassis_Yaw_gyro;
}Chassis_Message_t;
extern Chassis_Message_t Chassis_Message;
extern Send2Chassis_Info_t Send2chassis_Message;
extern Gimbal_Info_t gimbal_Ctrl;
extern Shoot_Info_t Shoot_Ctrl;

#endif


