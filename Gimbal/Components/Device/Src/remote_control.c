
//
// Created by YanYuanbin on 22-10-12.
//

#include "remote_control.h"

#include "usart.h"



rc_ctrl_t rc_ctrl;

uint8_t SBUS_rx_buf[2][SBUS_RX_BUF_NUM];

uint8_t UART1_TX_BUF[UART1_MAX_TX_LEN]={0};//���崮�ڷ��ͻ��棬��СΪ9
uint8_t UART1_RX_BUF[UART1_MAX_RX_LEN]={0};//���崮�ڽ��ջ���
uint8_t SBUS_MultiRx_Buf[2][SBUS_RX_BUF_NUM];
/*
 * �Զ����ͺ��� �������ݺͱ����������ж���
 * */
//void myprintf(int16_t value_1,int16_t value_2)
//{
//    UART1_TX_BUF[0] = 0xAA;
//    UART1_TX_BUF[1] = 0xBB;//֡ͷ
//    UART1_TX_BUF[2] = (uint8_t)(value_1 >> 8);
//    UART1_TX_BUF[3] = (uint8_t)(value_1);
//    UART1_TX_BUF[4] = (uint8_t)(value_2 >> 8);
//    UART1_TX_BUF[5] = (uint8_t)(value_2);

//		HAL_UART_Transmit_DMA(&huart1,UART1_TX_BUF,UART1_MAX_TX_LEN);
//}

static void rc_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
    //enable the DMA transfer for the receiver request
    //ʹ��DMA���ڽ���
    SET_BIT(huart3.Instance->CR3, USART_CR3_DMAR);

    //enalbe idle interrupt
    //ʹ�ܿ����ж�
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);

    //disable DMA
    //ʧЧDMA    
    do{
        __HAL_DMA_DISABLE(huart3.hdmarx);
    }while(huart3.hdmarx->Instance->CR & DMA_SxCR_EN);

    huart3.hdmarx->Instance->PAR = (uint32_t) & (USART3->DR);
    //memory buffer 
    //�ڴ滺����1
    huart3.hdmarx->Instance->M0AR = (uint32_t)(rx1_buf);
    //memory buffer 2
    //�ڴ滺����2
    huart3.hdmarx->Instance->M1AR = (uint32_t)(rx2_buf);
    //data length
    //���ݳ���
    huart3.hdmarx->Instance->NDTR = dma_buf_num;
    //enable double memory buffer
    //ʹ��˫������
    SET_BIT(huart3.hdmarx->Instance->CR, DMA_SxCR_DBM);

    //enable DMA
    //ʹ��DMA
    __HAL_DMA_ENABLE(huart3.hdmarx);
}

void remote_control_init(void)
{
    rc_init(SBUS_rx_buf[0], SBUS_rx_buf[1], SBUS_RX_BUF_NUM);
}

static void SBUS_TO_RC(volatile const uint8_t *sbus_buf, rc_ctrl_t  *rc_ctrl)
{
    if (sbus_buf == NULL || rc_ctrl == NULL) return;
		
    /* Channel 0, 1, 2, 3 */
    rc_ctrl->rc.ch[0] = (  sbus_buf[0]       | (sbus_buf[1] << 8 ) ) & 0x07ff;                            //!< Channel 0
    rc_ctrl->rc.ch[1] = ( (sbus_buf[1] >> 3) | (sbus_buf[2] << 5 ) ) & 0x07ff;                            //!< Channel 1
    rc_ctrl->rc.ch[2] = ( (sbus_buf[2] >> 6) | (sbus_buf[3] << 2 ) | (sbus_buf[4] << 10) ) & 0x07ff;      //!< Channel 2
    rc_ctrl->rc.ch[3] = ( (sbus_buf[4] >> 1) | (sbus_buf[5] << 7 ) ) & 0x07ff;                            //!< Channel 3
    rc_ctrl->rc.ch[4] = (  sbus_buf[16] 	 | (sbus_buf[17] << 8) ) & 0x07ff;                 			  //!< Channel 4
    /* Switch left, right */
    rc_ctrl->rc.s[0] = ((sbus_buf[5] >> 4) & 0x0003);                  //!< Switch left
    rc_ctrl->rc.s[1] = ((sbus_buf[5] >> 4) & 0x000C) >> 2;             //!< Switch right

    /* Mouse axis: X, Y, Z */
    rc_ctrl->mouse.x = sbus_buf[6] | (sbus_buf[7] << 8);                    //!< Mouse X axis
    rc_ctrl->mouse.y = sbus_buf[8] | (sbus_buf[9] << 8);                    //!< Mouse Y axis
    rc_ctrl->mouse.z = sbus_buf[10]| (sbus_buf[11] << 8);                  //!< Mouse Z axis

    /* Mouse Left, Right Is Press  */
    rc_ctrl->mouse.press_l = sbus_buf[12];                                  //!< Mouse Left Is Press
    rc_ctrl->mouse.press_r = sbus_buf[13];                                  //!< Mouse Right Is Press

    /* KeyBoard value */
    rc_ctrl->key.v = sbus_buf[14] | (sbus_buf[15] << 8);                    //!< KeyBoard value

    rc_ctrl->rc.ch[0] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[1] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[2] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[3] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[4] -= RC_CH_VALUE_OFFSET;
		
		rc_ctrl->online_Cnt = 250;
}

static void USAR_UART3_IDLECallback(UART_HandleTypeDef *huart)
{
    static uint16_t this_time_rx_len = 0;

    /* Current memory buffer used is Memory 0 */
    if(((((DMA_Stream_TypeDef  *)huart->hdmarx->Instance)->CR) & DMA_SxCR_CT )== 0U)
    {
        //ʧ��DMA�ж�
        __HAL_DMA_DISABLE(huart->hdmarx);
        //�õ���ǰʣ�����ݳ���
        this_time_rx_len = SBUS_RX_BUF_NUM - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        __HAL_DMA_SET_COUNTER(huart->hdmarx,SBUS_RX_BUF_NUM);
        huart->hdmarx->Instance->CR |= DMA_SxCR_CT;
        __HAL_DMA_ENABLE(huart->hdmarx);
        if(this_time_rx_len == RC_FRAME_LENGTH)
        {
            //����ң��������
            SBUS_TO_RC(SBUS_rx_buf[0], &rc_ctrl);
        }
    }else
    {
        //ʧ��DMA�ж�
        __HAL_DMA_DISABLE(huart->hdmarx);
        //�õ���ǰʣ�����ݳ���
        this_time_rx_len = SBUS_RX_BUF_NUM - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        __HAL_DMA_SET_COUNTER(huart->hdmarx,SBUS_RX_BUF_NUM);
        huart->hdmarx->Instance->CR &= ~(DMA_SxCR_CT);
        __HAL_DMA_ENABLE(huart->hdmarx);
        if(this_time_rx_len == RC_FRAME_LENGTH)
        {
            //����ң��������
            SBUS_TO_RC(SBUS_rx_buf[1], &rc_ctrl);
        }
    }
}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
		if(huart == NULL) return;
	
    // �ж��Ƿ��Ǵ���1
    if(huart->Instance == USART1)
    {	   // �ж��Ƿ��ǿ����ж�
        if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)
        {	 // ��������жϱ�־�������һֱ���Ͻ����жϣ�
            __HAL_UART_CLEAR_IDLEFLAG(huart);
            // �����жϴ�����
        }
    }
		// �ж��Ƿ��Ǵ���3
		else if(huart->Instance == USART3)
		{   // �ж��Ƿ��ǿ����ж�
		    if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)
        {	 // ��������жϱ�־�������һֱ���Ͻ����жϣ�
            __HAL_UART_CLEAR_IDLEFLAG(huart);
            // �����жϴ�����
            USAR_UART3_IDLECallback(huart);
        }
		}
}
