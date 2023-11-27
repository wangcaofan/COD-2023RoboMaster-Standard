 /* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : UI.c
  * @brief          : UI functions 
  * @author         : Yan Yuanbin
  * @date           : 2023/04/27
  * @version        : v1.0
  ******************************************************************************
  * @attention      : to be tested
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "ui.h"
#include "referee_info.h"
#include "stm32f4xx.h"
#include "crc.h"
#include "string.h"
#include "math.h"
#include "config.h"

/* Exported variables ---------------------------------------------------------*/
/**
  * @brief  structure variable of UI_FRAME_INFO_Typedef
  *         that contains the information of ui Transmit frame data
  */
UI_FRAME_INFO_Typedef UI_TxFrame_Info;

/* Private defines ------------------------------------------------------------*/
/**
  * @brief  the received ID of ui frame data
  */
#define Robot_Receiver_ID  (Referee_Info.robot_status.robot_id | 0x100)

/* Private variables ----------------------------------------------------------*/
/**
  * @brief  the full empty char
  */
static char empty_char[30]   = {"                              "};

/* Private function prototypes ------------------------------------------------*/



void UI_FrameInfo_Update(void)
{
  /* reset the graphic operate */
  if(UI_TxFrame_Info.Operate_Update_flag == 200)
  {
    UI_TxFrame_Info.graphic_operate = ADD;
    UI_TxFrame_Info.Operate_Update_flag = 0;
  }

  /* loop of ui frame info update */
  switch(UI_TxFrame_Info.Frame_Update_flag)
  {
    case 1:

    break;
    
    case 2:

		break;

    case 3:
        /* update the graphic operate */
        UI_TxFrame_Info.graphic_operate = MODIFY;
    break;

    default:
        /* reset the ui frame update flag */
        UI_TxFrame_Info.Frame_Update_flag = 0;
    break;
  }

  UI_TxFrame_Info.Frame_Update_flag++;
  UI_TxFrame_Info.Operate_Update_flag++;
}

/**
  * @brief  print the char graphic in the client
  * @param  graphic: pointer the structure of ext_client_string_t that 
  *                  contains the information of frame data.
  * @param  name: pointer the graphic name
  * @param  operate_tpye: the type of operate
  * @param  layer: the graphic printed layer
  * @param  color: the color of graphic
  * @param  size: the size of graphic
  * @param  length: the length of graphic
  * @param  width: the width of graphic
  * @param  start_x: the start x of graphic
  * @param  start_y: the start y of graphic
  * @param  character: pointer the character of graphic
  * @retval None
  */
static void Char_Graphic(ext_client_string_t* graphic,
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
	memcpy(graphic->data,empty_char,19);
    memcpy(graphic->data,character,length);
}

/**
  * @brief  print the char graphic in the client
  * @param  graphic: pointer the structure of ext_client_string_t that 
  *                  contains the information of frame data.
  * @param  name: pointer the graphic name
  * @param  operate_tpye: the type of operate
  * @param  graphic_tpye: 
  * @param  layer: the graphic printed layer
  * @param  color: the color of graphic
  * @param  start_angle: the start angle of graphic
  * @param  end_angle: the end angle of graphic
  * @param  width: the width of graphic
  * @param  start_x: the start x of graphic
  * @param  start_y: the start y of graphic
  * @param  radius: the radius of graphic
  * @param  end_x: the end x of graphic
  * @param  end_y: the end y of graphic
  * @retval None
  */
static void Figure_Graphic(graphic_data_struct_t* graphic,
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

