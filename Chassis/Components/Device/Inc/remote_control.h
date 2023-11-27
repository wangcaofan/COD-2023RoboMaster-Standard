//
// Created by YanYuanbin on 22-10-12.
//

#ifndef _MYUSART_H
#define _MYUSART_H

#include "stdint.h"
#include "stdbool.h"
#include "stm32f4xx.h"

#define UART1_MAX_TX_LEN    6U
#define UART1_MAX_RX_LEN    28U

#define SBUS_RX_BUF_NUM     36u
#define RC_FRAME_LENGTH     18u
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)


typedef  struct
{
    struct
    {
        int16_t ch[5];
        char s[2];
    } rc;
    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_l;
        uint8_t press_r;
    } mouse;
    union
    {
        uint16_t v;
        struct
        {
            uint16_t W:1;
            uint16_t S:1;
            uint16_t A:1;
            uint16_t D:1;
            uint16_t SHIFT:1;
            uint16_t CTRL:1;
            uint16_t Q:1;
            uint16_t E:1;
            uint16_t R:1;
            uint16_t F:1;
            uint16_t G:1;
            uint16_t Z:1;
            uint16_t X:1;
            uint16_t C:1;
            uint16_t V:1;
            uint16_t B:1;
        }set;
    }key;
		
		bool lost;
		uint8_t online_Cnt;
}rc_ctrl_t;

extern rc_ctrl_t rc_ctrl;

extern uint8_t SBUS_rx_buf[2][SBUS_RX_BUF_NUM];

extern uint8_t UART1_TX_BUF[UART1_MAX_TX_LEN];
extern uint8_t UART1_RX_BUF[UART1_MAX_RX_LEN];

extern void remote_control_init(void);
extern void myprintf(int16_t value_1,int16_t value_2);
extern void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
extern uint8_t SBUS_MultiRx_Buf[2][SBUS_RX_BUF_NUM];
#endif

