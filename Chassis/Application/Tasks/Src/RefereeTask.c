#include "RefereeTask.h"
#include "CANTxTask.h"
#include "UI.h"
extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;

/**
  * @brief          单字节解包
  * @param[in]      void
  * @retval         none
  */
static void referee_unpack_fifo_data(void);
static void REFEREE_Init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num);
extern UART_HandleTypeDef huart6;

uint8_t usart6_buf[2][USART_RX_BUF_LENGHT];
fifo_s_t referee_fifo;
uint8_t referee_fifo_buf[REFEREE_FIFO_BUF_LENGTH];
unpack_data_t referee_unpack_obj;

/**
  * @brief          裁判系统任务
  * @param[in]      pvParameters: NULL
  * @retval         none
  */
void Referee_Task(void const * argument)
{
  /* USER CODE BEGIN REFEREE_Task */
  init_referee_struct_data();
  fifo_s_init(&referee_fifo, referee_fifo_buf, REFEREE_FIFO_BUF_LENGTH);
  REFEREE_Init(usart6_buf[0], usart6_buf[1], USART_RX_BUF_LENGHT);
  /* Infinite loop */
  for(;;)
  {
      referee_unpack_fifo_data();
	
			Referee_Message.shoot_expectSpeed = robot_state.shooter_heat0_speed_limit;
  if(shoot_data_t.bullet_speed + 0.3f>robot_state.shooter_heat0_speed_limit)
    {
      Referee_Message.shoot_speedGain = (robot_state.shooter_heat0_speed_limit
                                              -  shoot_data_t.bullet_speed-0.3f)*100;
    }else Referee_Message.shoot_speedGain = 0;
     Referee_Message.shoot_remainingHeat = robot_state.shooter_heat0_cooling_limit - power_heat_data_t.shooter_heat0;
		if(Referee_Message.shoot_remainingHeat>250) Referee_Message.shoot_remainingHeat=250;
    osDelay(1);
  }
  /* USER CODE END REFEREE_Task */
}

void REFEREE_Init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
  //enable the DMA transfer for the receiver and tramsmit request
  SET_BIT(huart6.Instance->CR3, USART_CR3_DMAR);
  SET_BIT(huart6.Instance->CR3, USART_CR3_DMAT);

  //enalbe idle interrupt
  __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);

  //disable DMA
  __HAL_DMA_DISABLE(&hdma_usart6_rx);

  while(hdma_usart6_rx.Instance->CR & DMA_SxCR_EN)
  {
    __HAL_DMA_DISABLE(&hdma_usart6_rx);
  }

  __HAL_DMA_CLEAR_FLAG(&hdma_usart6_rx, DMA_LISR_TCIF1);

  hdma_usart6_rx.Instance->PAR = (uint32_t) & (USART6->DR);
  //memory buffer 1
  hdma_usart6_rx.Instance->M0AR = (uint32_t)(rx1_buf);
  //memory buffer 2
  hdma_usart6_rx.Instance->M1AR = (uint32_t)(rx2_buf);
  //data length
  __HAL_DMA_SET_COUNTER(&hdma_usart6_rx, dma_buf_num);

  //enable double memory buffer
  SET_BIT(hdma_usart6_rx.Instance->CR, DMA_SxCR_DBM);

  //enable DMA
  __HAL_DMA_ENABLE(&hdma_usart6_rx);


  //disable DMA
  __HAL_DMA_DISABLE(&hdma_usart6_tx);

  while(hdma_usart6_tx.Instance->CR & DMA_SxCR_EN)
  {
    __HAL_DMA_DISABLE(&hdma_usart6_tx);
  }

  hdma_usart6_tx.Instance->PAR = (uint32_t) & (USART6->DR);
}

void REFEREE_IRQHandler_Called(void)
{
  static volatile uint8_t res;
  if(USART6->SR & UART_FLAG_IDLE)
  {
    __HAL_UART_CLEAR_PEFLAG(&huart6);

    static uint16_t this_time_rx_len = 0;

    if((huart6.hdmarx->Instance->CR & DMA_SxCR_CT) == RESET)
    {
      __HAL_DMA_DISABLE(huart6.hdmarx);
      this_time_rx_len = USART_RX_BUF_LENGHT - __HAL_DMA_GET_COUNTER(huart6.hdmarx);
      __HAL_DMA_SET_COUNTER(huart6.hdmarx, USART_RX_BUF_LENGHT);
      huart6.hdmarx->Instance->CR |= DMA_SxCR_CT;
      __HAL_DMA_ENABLE(huart6.hdmarx);
      fifo_s_puts(&referee_fifo, (char*)usart6_buf[0], this_time_rx_len);
    }
    else
    {
      __HAL_DMA_DISABLE(huart6.hdmarx);
      this_time_rx_len = USART_RX_BUF_LENGHT - __HAL_DMA_GET_COUNTER(huart6.hdmarx);
      __HAL_DMA_SET_COUNTER(huart6.hdmarx, USART_RX_BUF_LENGHT);
      huart6.hdmarx->Instance->CR &= ~(DMA_SxCR_CT);
      __HAL_DMA_ENABLE(huart6.hdmarx);
      fifo_s_puts(&referee_fifo, (char*)usart6_buf[1], this_time_rx_len);
    }
  }
}


/**
  * @brief          单字节解包
  * @param[in]      void
  * @retval         none
  */
void referee_unpack_fifo_data(void)
{
  uint8_t byte = 0;
  uint8_t sof = HEADER_SOF;
  unpack_data_t *p_obj = &referee_unpack_obj;

  while ( fifo_s_used(&referee_fifo) )
  {
    byte = fifo_s_get(&referee_fifo);
    switch(p_obj->unpack_step)
    {
      case STEP_HEADER_SOF:
      {
        if(byte == sof)
        {
          p_obj->unpack_step = STEP_LENGTH_LOW;
          p_obj->protocol_packet[p_obj->index++] = byte;
        }
        else
        {
          p_obj->index = 0;
        }
      }break;
      
      case STEP_LENGTH_LOW:
      {
        p_obj->data_len = byte;
        p_obj->protocol_packet[p_obj->index++] = byte;
        p_obj->unpack_step = STEP_LENGTH_HIGH;
      }break;
      
      case STEP_LENGTH_HIGH:
      {
        p_obj->data_len |= (byte << 8);
        p_obj->protocol_packet[p_obj->index++] = byte;

        if(p_obj->data_len < (REF_PROTOCOL_FRAME_MAX_SIZE - REF_HEADER_CRC_CMDID_LEN))
        {
          p_obj->unpack_step = STEP_FRAME_SEQ;
        }
        else
        {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;
        }
      }break;
      case STEP_FRAME_SEQ:
      {
        p_obj->protocol_packet[p_obj->index++] = byte;
        p_obj->unpack_step = STEP_HEADER_CRC8;
      }break;

      case STEP_HEADER_CRC8:
      {
        p_obj->protocol_packet[p_obj->index++] = byte;

        if (p_obj->index == REF_PROTOCOL_HEADER_SIZE)
        {
          if ( verify_CRC8_check_sum(p_obj->protocol_packet, REF_PROTOCOL_HEADER_SIZE) )
          {
            p_obj->unpack_step = STEP_DATA_CRC16;
          }
          else
          {
            p_obj->unpack_step = STEP_HEADER_SOF;
            p_obj->index = 0;
          }
        }
      }break;  
      
      case STEP_DATA_CRC16:
      {
        if (p_obj->index < (REF_HEADER_CRC_CMDID_LEN + p_obj->data_len))
        {
           p_obj->protocol_packet[p_obj->index++] = byte;  
        }
        if (p_obj->index >= (REF_HEADER_CRC_CMDID_LEN + p_obj->data_len))
        {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;

          if ( verify_CRC16_check_sum(p_obj->protocol_packet, REF_HEADER_CRC_CMDID_LEN + p_obj->data_len) )
          {
            referee_data_solve(p_obj->protocol_packet);
          }
        }
      }break;

      default:
      {
        p_obj->unpack_step = STEP_HEADER_SOF;
        p_obj->index = 0;
      }break;
    }
  }
}

