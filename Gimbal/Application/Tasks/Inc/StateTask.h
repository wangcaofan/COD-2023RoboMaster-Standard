//
// Created by YanYuanbin on 22-10-5.
//

#ifndef _SERIALTASK_H
#define _SERIALTASK_H

#include "stdint.h"
#include "string.h"
#include "stdbool.h"

//系统状态枚举
typedef enum
{
	Disable = 0x00U,  //失能
	Enable  = 0x01U,	//使能
	Calibrate = 0x02U,//校准
	Rclost  = 0x03U,  //遥控器离线
}SystemState_e;

//系统状态信息结构体，待完善
typedef struct
{
	SystemState_e state;


}System_Info_t;


#endif


