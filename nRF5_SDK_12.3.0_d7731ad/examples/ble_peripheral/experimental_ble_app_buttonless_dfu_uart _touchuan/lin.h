/*
 *主机Lin通信声明
 */

#ifndef _LIN_H
#define _LIN_H

//#include "misc.h"
//#include "stm32f10x.h"
//#include "stm32f4xx.h"
//#include "stm32f4xx_rcc.h"
//#include "stm32f4xx_gpio.h"
//#include "stm32f4xx_usart.h"
#include <stdint.h>

typedef struct {
    char     enable;    // ID是否有效
    char     update;    // ID是否更新
    char     press;     // ID发布
    char     receive;   // ID接收
    char     send_one_time_enable;   //  是否偶发信号  1 定时发  0 偶发
    volatile char     send_one_time_active;   //  发送一次
    uint8_t  data[8];
    
} lin_data_t;

#define IS_SLAVE        0       // 是否从机
#define LIN_DATA_LEN    0x40
extern lin_data_t lin_data[LIN_DATA_LEN];


//#define LIN_TX_Pin		GPIO_Pin_2
//#define LIN_TX_Port		GPIOA
//#define LIN_TX_CLK		RCC_APB2Periph_GPIOA

//#define LIN_RX_Pin		GPIO_Pin_3
//#define LIN_RX_Port		GPIOA
//#define LIN_TX_CLK		RCC_APB2Periph_GPIOA

//#define LIN_UART		USART2
//#define LIN_UART_CLK	RCC_APB1Periph_USART2

//void Lin_Config(void);

//void Lin_SendHeader(uint8_t ID);

//void Lin_SendData(uint8_t *,uint8_t);

uint8_t Lin_CheckPID(uint8_t PID);

uint8_t Lin_Check_Sum(uint8_t* Checksum_Buffer, uint8_t Data_Number);

void Lin_data_init(void);

// 主机发布一次
int Lin_ID_data_press(const uint8_t ID, const uint8_t data[8]);

// 主机定时调用
void Lin_master_go(void);

void Lin_data_ready(uint8_t byte);

void Lin_data_tx_done(void);

//// 检查控制参数，越界关闭平衡电机
//void CheckRunLimit(void);

#endif
