/*
 *主机Lin通信声明
 */

#ifndef _LIN_H
#define _LIN_H

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

#define LIN_DATA_LEN    0x40
extern lin_data_t lin_data[LIN_DATA_LEN];

uint8_t Lin_CheckPID(uint8_t PID);

uint8_t Lin_Check_Sum(uint8_t* buf, uint8_t n);

void Lin_data_init(void);

uint8_t Lin_ID_to_len(uint8_t ID);

// 主机发布一次
int Lin_ID_data_press(const uint8_t ID, const uint8_t data[8]);

// 主机定时调用
void Lin_master_go(void);

void Lin_data_ready(uint8_t byte);

void Lin_data_tx_done(void);

#endif
