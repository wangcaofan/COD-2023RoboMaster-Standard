//
// Created by YanYuanbin on 22-10-5.
//

#ifndef _SERIALTASK_H
#define _SERIALTASK_H

#include "stdint.h"
#include "string.h"
#include "stdbool.h"

//ϵͳ״̬ö��
typedef enum
{
	Disable = 0x00U,  //ʧ��
	Enable  = 0x01U,	//ʹ��
	Calibrate = 0x02U,//У׼
	Rclost  = 0x03U,  //ң��������
}SystemState_e;

//ϵͳ״̬��Ϣ�ṹ�壬������
typedef struct
{
	SystemState_e state;


}System_Info_t;


#endif


