#ifndef _CANTXTASK_H
#define _CANTXTASK_H

#include "main.h"
typedef struct{
      uint8_t allState;
    
      uint8_t SupercapWork;
    
      uint8_t chassisWork;
     
      uint8_t chassisMode;
    
      int16_t vx,vy;
	
	    uint8_t ShootWorkMode;
 }Gimbal_Message_t; 
extern Gimbal_Message_t Gimbal2Chassis;

typedef  struct{
      int16_t shoot_remainingHeat;
      int16_t shoot_speedGain;
      uint16_t shoot_expectSpeed;
}Referee_Message_t;
extern Referee_Message_t Referee_Message;
typedef  struct{
  	  int16_t Cap_low_waring;
			int16_t DischargeATK;
			int16_t Chassis_Power;
			int16_t CapQuantity;
	    uint8_t SuperCapSwitch;
}SuperCap_Info_t;
extern SuperCap_Info_t SuperCap_Info;
typedef struct{
  uint16_t Power_limit;
  uint16_t Power_buffer;
  uint16_t Powerbuffer_limit;
}Send2SuperCap_Info_t; 
extern Send2SuperCap_Info_t Send2SuperCap_Info;
#endif
