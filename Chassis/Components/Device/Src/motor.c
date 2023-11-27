//
// Created by YanYuanbin on 22-10-5.
//

#include "motor.h"

DJI_MOTOR Chassis_Motor[MOTOR_NUM]=
{
	[Right_B] = {
			.CANx = CAN2,
			.type = _3508,
			.Data.StdId = 0x201,
			.usage = Right_B,
	},
	[Right_F] = {
			.CANx = CAN2,
			.type = _3508,
			.Data.StdId = 0x202,
			.usage = Right_F,
	},
	[Left_F] = {
			.CANx = CAN2,
			.type = _3508,
			.Data.StdId = 0x203,
			.usage =Left_F ,
	},
	[Left_B] = {
			.CANx = CAN2,
			.type = _3508,
			.Data.StdId = 0x204,
			.usage = Left_B,
	},
	[Yaw] = {
			.CANx = CAN1,
			.type = _6020,
			.Data.StdId = 0x205,
			.usage = Yaw,
	},
};

static float encoder2Angle(MOTOR_USAGE usage,int16_t act_Encoder,float torque_Ratio)
{
    static int32_t angle_Last[MOTOR_NUM];
    static int32_t result_Encoder[MOTOR_NUM];
    int res1,res2;		
	
    if(act_Encoder<angle_Last[usage])
		{
        res1 = act_Encoder-angle_Last[usage]+8192;
        res2 = act_Encoder-angle_Last[usage];
    }else
		{
        res1=act_Encoder-angle_Last[usage]-8192;
        res2=act_Encoder-angle_Last[usage];
    }
		//更新上一次编码角
    angle_Last[usage]=act_Encoder;
		
    ABS(res1)<ABS(res2) ? (result_Encoder[usage]+=res1) : (result_Encoder[usage]+=res2);
		
    if(usage == _3508){
        ABS(result_Encoder[usage]) > 8192*torque_Ratio ? (result_Encoder[usage]-result_Encoder[usage]/ABS(result_Encoder[usage])*8192*torque_Ratio): result_Encoder[usage];
    }
    return (float)result_Encoder[usage]/(8192.f*torque_Ratio)*360.0f;
}

void get_Motor_Data(uint32_t *canId, uint8_t *rxBuf,DJI_MOTOR* DJI_Motor)
{
    if(*canId !=DJI_Motor->Data.StdId) return;
    //基础信息
    DJI_Motor->Data.temperature = rxBuf[6];
    DJI_Motor->Data.encoder  = ((int16_t)rxBuf[0]<<8 | (int16_t)rxBuf[1]);
    DJI_Motor->Data.velocity = ((int16_t)rxBuf[2]<<8 | (int16_t)rxBuf[3]);
    DJI_Motor->Data.current  = ((int16_t)rxBuf[4]<<8 | (int16_t)rxBuf[5]);

    //按电机类型做特殊处理
    switch (DJI_Motor->type)
    {
        case _6020:
            DJI_Motor->Data.angle = (DJI_Motor->Data.encoder/8192.f*360.f);
            break;
 
        case _2006:
            DJI_Motor->Data.angle = encoder2Angle(DJI_Motor->usage,DJI_Motor->Data.encoder,36.0f);
            break;

        case _3508:
            DJI_Motor->Data.angle = encoder2Angle(DJI_Motor->usage,DJI_Motor->Data.encoder,3591.f/187.f);
            break;
				
				case _3510:
            DJI_Motor->Data.angle = encoder2Angle(DJI_Motor->usage,DJI_Motor->Data.encoder,19.f);
				break;

        default:
					return;
				break;
    }
}
