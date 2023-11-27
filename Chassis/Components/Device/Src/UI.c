#include "UI.h"
#include "usart.h"
#include "Referee.h"
#include "ChassisTask.h"
#include "CANTxTask.h"
#include "arm_math.h"
#define Robot_Receiver_ID (robot_state.robot_id | 0x100)





ext_charstring_data_t tx_client_char;
ext_graphic_five_data_t tx_client_line;

uint8_t update_draw_flag;

uint8_t state_first_graphic = 0 ;
uint8_t CliendTxBuffer[200];
char first_line[30] = {"SPIN"};
char second_line[30]= {"CAP"};
char third_line[30] = {"AUTO"};
char fourth_line[30]= {"COVER"};
char empty_line[30]   = {"                              "};

void Char_Graphic(ext_client_string_t* graphic,
                    const char* name,
                    uint32_t operate_tpye,
                    uint32_t layer,
                    uint32_t color,
                    uint32_t size,
                    uint32_t length,
                    uint32_t width,
                    uint32_t start_x,
                    uint32_t start_y,
                    const char *character)
{
	graphic_data_struct_t *data_struct = &graphic->grapic_data_struct;
	for(char i=0;i<3;i++)
		data_struct->graphic_name[i] = name[i];	
	data_struct->operate_tpye = operate_tpye; 
	data_struct->graphic_tpye = CHAR;         
	data_struct->layer = layer;
	data_struct->color = color;
	data_struct->start_angle = size;
	data_struct->end_angle = length;	
	data_struct->width = width;
	data_struct->start_x = start_x;
	data_struct->start_y = start_y;	
	
	data_struct->radius = 0;
	data_struct->end_x = 0;
	data_struct->end_y = 0;
	memcpy(graphic->data,empty_line,19);
    memcpy(graphic->data,character,length);
}
void Figure_Graphic(graphic_data_struct_t* graphic,
									const char* name,
									uint32_t operate_tpye,
									uint32_t graphic_tpye,
									uint32_t layer,
									uint32_t color,
									uint32_t start_angle,
									uint32_t end_angle,
									uint32_t width,
									uint32_t start_x,
									uint32_t start_y,
									uint32_t radius,
									uint32_t end_x,
									uint32_t end_y)							
{
	for(char i=0;i<3;i++)
	graphic->graphic_name[i] = name[i];	
	graphic->operate_tpye = operate_tpye; 
	graphic->graphic_tpye = graphic_tpye;     
	graphic->layer        = layer;
	graphic->color        = color;
	graphic->start_angle  = start_angle;
	graphic->end_angle    = end_angle;	
	graphic->width        = width;
	graphic->start_x      = start_x;
	graphic->start_y      = start_y;	
	graphic->radius = radius;
	graphic->end_x  = end_x;
	graphic->end_y  = end_y;
}

static void Draw_char()
{
  if(state_first_graphic == 0)
	{
		if(Chassis_Ctrl.workMode == 1)
		{
			Char_Graphic(&tx_client_char.clientData,"CL1",update_draw_flag,0,GREEN,15,strlen(first_line),2,(30),(1080*16/20),first_line);//x1920/18
		}
		else
		{
			Char_Graphic(&tx_client_char.clientData,"CL1",update_draw_flag,0,YELLOW,15,strlen(first_line),2,(30),(1080*16/20),first_line);//x1920/18
		}
		state_first_graphic =1;
	}
  else if(state_first_graphic == 1)
	{
		if(SuperCap_Info.DischargeATK == 0)
		{
			Char_Graphic(&tx_client_char.clientData,"CL2",update_draw_flag,0,GREEN,15,strlen(second_line),2,(30),(1080*15/20),second_line);//x1920/18
		}
		else
		{
			Char_Graphic(&tx_client_char.clientData,"CL2",update_draw_flag,0,YELLOW,15,strlen(second_line),2,(30),(1080*15/20),second_line);//x1920/18
		}
		state_first_graphic =2;
	}
	else if(state_first_graphic == 2)
	{
		if(Gimbal2Chassis.ShootWorkMode == 1)
		{
			Char_Graphic(&tx_client_char.clientData,"CL3",update_draw_flag,0,GREEN,15,strlen(third_line),2,(30),(1080*14/20),third_line);//x1920/18
		}
		else
		{
			Char_Graphic(&tx_client_char.clientData,"CL3",update_draw_flag,0,YELLOW,15,strlen(third_line),2,(30),(1080*14/20),third_line);//x1920/18
		}
		state_first_graphic =3;
	}
}

void Client_Char_Init(void)
{
	if(state_first_graphic >= 3)
	{
		state_first_graphic = 0;
	}
    tx_client_char.txFrameHeader.SOF = 0xA5;
    tx_client_char.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(ext_client_string_t);
    tx_client_char.txFrameHeader.Seq = 0;
    memcpy(CliendTxBuffer,&tx_client_char.txFrameHeader,sizeof(xFrameHeader));
    append_CRC8_check_sum(CliendTxBuffer, sizeof(xFrameHeader));

    tx_client_char.CmdID = 0x0301;
    
    tx_client_char.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_char_graphic;
    tx_client_char.dataFrameHeader.send_ID     = robot_state.robot_id;
    tx_client_char.dataFrameHeader.receiver_ID = Robot_Receiver_ID;
    
    Draw_char();
    memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_client_char.CmdID, LEN_CMD_ID+tx_client_char.txFrameHeader.DataLength);
    
    append_CRC16_check_sum(CliendTxBuffer,sizeof(tx_client_char));

    HAL_UART_Transmit_DMA(&huart6,CliendTxBuffer,sizeof(tx_client_char));
}

  int16_t LINE_OFFSET[2] = {0,};

static void Draw_line(void)
{
//	Figure_Graphic(&tx_client_line.clientData[0],"AL1",ADD,LINE,3,YELLOW,0,0,3,540,0, 0,640,100);
//	Figure_Graphic(&tx_client_line.clientData[1],"AL2",ADD,LINE,3,YELLOW,0,0,3,1380,0, 0,1280,100);

		
	if	(SuperCap_Info.CapQuantity >= 60){
		Figure_Graphic(&tx_client_line.clientData[4],"AL5",update_draw_flag,LINE,2,GREEN,0,0,10,670,20  ,0, 640+(SuperCap_Info.CapQuantity-48)*6.2,20);
	} else Figure_Graphic(&tx_client_line.clientData[4],"AL5",update_draw_flag,LINE,2,FUCHSIA,0,0,10,670,20  ,0,640+200,20);
  }


void Client_Graph_Init(void)
{
	tx_client_line.txFrameHeader.SOF = JUDGE_FRAME_HEADER;
	tx_client_line.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t)*5;
	tx_client_line.txFrameHeader.Seq = 0;
	memcpy(CliendTxBuffer,&tx_client_line.txFrameHeader,sizeof(xFrameHeader));
	append_CRC8_check_sum(CliendTxBuffer, sizeof(xFrameHeader));

	tx_client_line.CmdID = 0x301;

	tx_client_line.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_seven_graphic;
	tx_client_line.dataFrameHeader.send_ID     = robot_state.robot_id;
	tx_client_line.dataFrameHeader.receiver_ID = Robot_Receiver_ID;

	Draw_line();
	memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_client_line.CmdID, LEN_CMD_ID+tx_client_line.txFrameHeader.DataLength);

	append_CRC16_check_sum(CliendTxBuffer,sizeof(tx_client_line));
		
    HAL_UART_Transmit_DMA(&huart6,CliendTxBuffer,sizeof(tx_client_line));
}


void Startjudge_task(void)
{	
  static uint8_t i = 0, J = 0;

  if(J == 250)
  {
    update_draw_flag = ADD;
    J = 0;
  }

  switch(i)
  {
    case 1:
        Client_Char_Init();
    break;
    
    case 2:
        Client_Graph_Init();
        update_draw_flag = MODIFY;
		break;

    default:
        i = 0;
    break;
  }

  i++;
  J++;
}

