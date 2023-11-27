//
// Created by YanYuanbin on 22-10-5.
//

#include "motor.h"

DJI_MOTOR Gimbal_Motor[MOTOR_NUM]=
{
	[Pitch] = {
			.CANx = CAN1,
			.type = _6020,
			.Data.StdId = 0x206,
			.usage = Pitch,
	},
	[Yaw] = {
			.CANx = CAN1,
			.type = _6020,
			.Data.StdId = 0x205,
			.usage = Yaw,
	},
	[SHOOTL] = {
			.CANx = CAN2,
			.type = _3508,
			.Data.StdId = 0x201,
			.usage = Pitch,
	},
	[SHOOTR] = {
			.CANx = CAN2,
			.type = _3508,
			.Data.StdId = 0x202,
			.usage = Yaw,
	},
	[TRIGGER] = {
			.CANx = CAN2,
			.type = _2006,
			.Data.StdId = 0x203,
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
static float DJI_MotorEncoder2Angle(DJI_MOTOR* motor,float torque_Ratio)
{
	static int32_t angle_Last[MOTOR_NUM];
	static int32_t result_Encoder[MOTOR_NUM];
	int res1,res2;
	if(motor->Data.encoder < angle_Last[motor->usage])
	{
		res1 = motor->Data.encoder - angle_Last[motor->usage]+8192;
		res2 = motor->Data.encoder - angle_Last[motor->usage];
	}
	else
	{
		res1 = motor->Data.encoder - angle_Last[motor->usage]-8192;
		res2 = motor->Data.encoder - angle_Last[motor->usage];
	}
	angle_Last[motor->usage] = motor->Data.encoder;
	ABS(res1)<ABS(res2) ? (result_Encoder[motor->usage]+=res1) : (result_Encoder[motor->usage]+=res2);
	if(result_Encoder[motor->usage]>8192*torque_Ratio-1) result_Encoder[motor->usage]=0;
	if(result_Encoder[motor->usage]<0) result_Encoder[motor->usage]=8192*torque_Ratio-1;
	
	return result_Encoder[motor->usage]/(8192*torque_Ratio)*360.0f;
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
            DJI_Motor->Data.angle = encoder2Angle(DJI_Motor->usage,DJI_Motor->Data.encoder,1.0f);
            break;

        case _2006:
            DJI_Motor->Data.angle = DJI_MotorEncoder2Angle(DJI_Motor,36.0f);
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
