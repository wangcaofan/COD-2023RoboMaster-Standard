#ifndef _UI_H_
#define _UI_H_
#include "stm32f4xx.h"
#include "referee.h"
#include "crc.h"
#include "stdbool.h"

typedef struct
{
	uint8_t SPIN;
	uint8_t FRI;
	uint8_t FLY;
	uint8_t AUTO;
	uint8_t COVER;
	uint8_t SENTRY;
  float Vcap_show;

}User_CMD_t;


typedef struct
{
  bool IF_Init_Over;
  User_CMD_t User;
}UI_Info_t;

#define JUDGE_FRAME_HEADER 0xA5           //帧头


enum
{
	//0x200-0x02ff 	队伍自定义命令 格式  INTERACT_ID_XXXX
	INTERACT_ID_delete_graphic 			= 0x0100,	/*客户端删除图形*/
	INTERACT_ID_draw_one_graphic 		= 0x0101,	/*客户端绘制一个图形*/
	INTERACT_ID_draw_two_graphic 		= 0x0102,	/*客户端绘制2个图形*/
	INTERACT_ID_draw_five_graphic 		= 0x0103,	/*客户端绘制5个图形*/
	INTERACT_ID_draw_seven_graphic 		= 0x0104,	/*客户端绘制7个图形*/
	INTERACT_ID_draw_char_graphic 		= 0x0110,	/*客户端绘制字符图形*/
	INTERACT_ID_bigbome_num				= 0x02ff
};
//单位（字节）
enum
{
	LEN_INTERACT_delete_graphic     = 8,  //删除图层 2(数据内容ID)+2(发送者ID)+2（接收者ID）+2（数据内容）  
	LEN_INTERACT_draw_one_graphic   = 21, // 以上2+2+2+15
	LEN_INTERACT_draw_two_graphic   = 36, //6+15*2
	LEN_INTERACT_draw_five_graphic  = 81, //6+15*5
	LEN_INTERACT_draw_seven_graphic = 111,//6+15*7
	LEN_INTERACT_draw_char_graphic  = 51, //6+15+30（字符串内容）
};
//****************************绘图的数据段内容****************************/
typedef __packed struct//图形
{                          
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3; //直线  矩形  正圆  椭圆  圆弧  浮点  整型  字符
	uint32_t layer:4;        
	uint32_t color:4;        
	uint32_t start_angle:9;  //空    空    空    空    角度  大小  大小  大小
	uint32_t end_angle:9;    //空    空    空    空          位数  空    长度
	uint32_t width:10;       
	uint32_t start_x:11;     //起点  起点  圆心  圆心  圆心  起点  起点  起点
	uint32_t start_y:11;     //
	uint32_t radius:10;      //空    空    半径  空    空    、    、    空
	uint32_t end_x:11;       //终点  对顶  空    半轴  半轴  、    、    空
	uint32_t end_y:11;       //                              数    数    空
} graphic_data_struct_t;
typedef __packed struct//浮点数
{                          
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3; 
	uint32_t layer:4;        
	uint32_t color:4;        
	uint32_t start_angle:9;  
	uint32_t end_angle:9;    
	uint32_t width:10;       
	uint32_t start_x:11;    
	uint32_t start_y:11;     
	float number;       
} Float_data_struct_t;
typedef __packed struct//整型数
{                          
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3; 
	uint32_t layer:4;        
	uint32_t color:4;        
	uint32_t start_angle:9;  
	uint32_t end_angle:9;    
	uint32_t width:10;       
	uint32_t start_x:11;    
	uint32_t start_y:11;     
  int number;       
} Int_data_struct_t;
/* data_ID: 0X0100  Byte:  2	    客户端删除图形*/
typedef __packed struct
{
	uint8_t operate_type; 
	uint8_t layer;//图层数：0~9
}ext_client_custom_graphic_delete_t;
typedef enum
{
	NONE_delete    = 0,
	GRAPHIC_delete = 1,
	ALL_delete     = 2
}delete_Graphic_Operate;//ext_client_custom_graphic_delete_t：uint8_t operate_type
/*图层删除操作*/

//bit 0-2
typedef enum
{
	NONE   = 0,/*空操作*/
	ADD    = 1,/*增加图层*/
	MODIFY = 2,/*修改图层*/
	DELETE = 3,/*删除图层*/
}Graphic_Operate;//graphic_data_struct_t：uint32_t operate_tpye
/*图层操作*/
//bit3-5
typedef enum
{
	LINE      = 0,//直线
	RECTANGLE = 1,//矩形
	CIRCLE    = 2,//正圆
	OVAL      = 3,//椭圆
	ARC       = 4,//圆弧
	FLOAT     = 5,//浮点数
	INT       = 6,//整型数
	CHAR      = 7 //字符
}Graphic_Type;
/*图层类型*/
//bit 6-9图层数 最大为9，最小0
//bit 10-13颜色
typedef enum
{
	RED_BLUE  = 0,//红蓝主色	
	YELLOW    = 1,
	GREEN     = 2,
	ORANGE    = 3,
	FUCHSIA   = 4,	/*紫红色*/
	PINK      = 5,
	CYAN_BLUE = 6,	/*青色*/
	BLACK     = 7,
	WHITE     = 8
}Graphic_Color;
/*图层颜色类型*/
//bit 14-31 角度 [0,360]
/**********************************客户端绘图************************************************/
//删除图层
/* 自定义帧头 */
typedef __packed struct
{
	uint8_t  SOF;
	uint16_t DataLength;
	uint8_t  Seq;
	uint8_t  CRC8;
	
} xFrameHeader;

/* 交互数据接收信息：0x0301  */
typedef __packed struct 
{ 
	uint16_t data_cmd_id;    
	uint16_t send_ID;    
	uint16_t receiver_ID; 
} ext_student_interactive_header_data_t; 


typedef __packed struct
{
	xFrameHeader txFrameHeader;			
	uint16_t  CmdID;										
	ext_student_interactive_header_data_t   dataFrameHeader;
	ext_client_custom_graphic_delete_t clientData;		
	uint16_t	FrameTail;								
}ext_deleteLayer_data_t;

//绘字符串
typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct;
	uint8_t data[30];
} ext_client_string_t;

//固定数据段长度数据包
typedef __packed struct
{
	xFrameHeader txFrameHeader;			//帧头
	uint16_t  CmdID;										//命令码
	ext_student_interactive_header_data_t   dataFrameHeader;//数据段头结构
	ext_client_string_t clientData;//数据段
	uint16_t	FrameTail;								//帧尾
}ext_charstring_data_t;
//绘象形图
typedef __packed struct
{
	xFrameHeader txFrameHeader;			//帧头
	uint16_t  CmdID;										//命令码
	ext_student_interactive_header_data_t   dataFrameHeader;//数据段头结构
	graphic_data_struct_t clientData;		//数据段
	uint16_t	FrameTail;								//帧尾
}ext_graphic_one_data_t;
typedef __packed struct
{
	xFrameHeader txFrameHeader;			
	uint16_t  CmdID;										
	ext_student_interactive_header_data_t   dataFrameHeader;
	graphic_data_struct_t clientData[2];		
	uint16_t	FrameTail;								
}ext_graphic_two_data_t;
typedef __packed struct
{
	xFrameHeader txFrameHeader;			
	uint16_t  CmdID;										
	ext_student_interactive_header_data_t   dataFrameHeader;
	graphic_data_struct_t clientData[5];		
	uint16_t	FrameTail;								
}ext_graphic_five_data_t;
typedef __packed struct
{
	xFrameHeader txFrameHeader;			
	uint16_t  CmdID;										
	ext_student_interactive_header_data_t   dataFrameHeader;
	graphic_data_struct_t clientData[7];		
	uint16_t	FrameTail;								
}ext_graphic_seven_data_t;
//绘制浮点型
typedef __packed struct
{
	xFrameHeader txFrameHeader;			
	uint16_t  CmdID;										
	ext_student_interactive_header_data_t   dataFrameHeader;
	Float_data_struct_t clientData[2];		
	uint16_t	FrameTail;								
}ext_float_two_data_t;
typedef __packed struct
{
	xFrameHeader txFrameHeader;			
	uint16_t  CmdID;										
	ext_student_interactive_header_data_t   dataFrameHeader;
	Float_data_struct_t clientData[7];		
	uint16_t	FrameTail;								
}ext_float_seven_data_t;
//绘制整型
typedef __packed struct
{
	xFrameHeader txFrameHeader;			
	uint16_t  CmdID;										
	ext_student_interactive_header_data_t   dataFrameHeader;
	Int_data_struct_t clientData[2];		
	uint16_t	FrameTail;								
}ext_int_two_data_t;
typedef __packed struct
{
	xFrameHeader txFrameHeader;			
	uint16_t  CmdID;										
	ext_student_interactive_header_data_t   dataFrameHeader;
	Int_data_struct_t clientData[7];		
	uint16_t	FrameTail;								
}ext_int_seven_data_t;

typedef enum
{
	/* Std */
	LEN_FRAME_HEAD 	                 = 5,	// 帧头长度
	LEN_CMD_ID 		                   = 2,	// 命令码长度
	LEN_FRAME_TAIL 	                 = 2,	// 帧尾CRC16
	/* Ext */  

	LEN_game_state       				=  11,	//0x0001
	LEN_game_result       				=  1,	//0x0002
	LEN_game_robot_survivors       		=  32,	//0x0003  比赛机器人血量数据
	LED_game_missile_state      =3  , //0X0004飞镖发射状态
	LED_game_buff               =11 , //0X0005
	
	LEN_event_data  					=  4,	//0x0101  场地事件数据 
	LEN_supply_projectile_action        =  4,	//0x0102场地补给站动作标识数据
	LEN_supply_warm        =2, //裁判系统警告 0x0104
	LEN_missile_shoot_time =1  , //飞镖发射口倒计时
	
	LEN_game_robot_state    			= 27,	//0x0201机器人状态数据
	LEN_power_heat_data   				= 16,	//0x0202实时功率热量数据
	LEN_game_robot_pos        			= 16,	//0x0203机器人位置数据
	LEN_buff_musk        				=  1,	//0x0204机器人增益数据
	LEN_aerial_robot_energy        		=  1,	//0x0205空中机器人能量状态数据
	LEN_robot_hurt        				=  1,	//0x0206伤害状态数据
	LEN_shoot_data       				=  7,	//0x0207	实时射击数据
	LEN_bullet_remaining          = 6,//剩余发射数
  
	LEN_rfid_status					         = 4,
	LEN_dart_client_directive        = 12,//0x020A
	// 0x030x
	//LEN_robot_interactive_header_data      = n,
	//LEN_controller_interactive_header_data = n,
	LEN_map_interactive_headerdata           = 15,
	LEN_keyboard_information                 = 12,//0x0304

}JudgeDataLength;

extern UI_Info_t UI;

void Startjudge_task(void);
#endif


