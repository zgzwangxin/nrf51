/*
 *主机LIN通信函数定义
 */
 
#include "lin.h"
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "app_uart.h"
#include "nrf_uart.h"
#include "nrf_gpio.h"

#include "global.h"


lin_data_t lin_data[LIN_DATA_LEN] = {0};

uint8_t Lin_data_send_buf[13] = {0};
uint8_t Lin_data_send_idx = 0;

volatile static uint8_t Lin_execute_current_ID;

static uint8_t Lin_init_flag = 0;

#define Lin_FLAG_data_init  0x01

static bool volatile uart_data_sending = false;
static bool volatile lin_data_sending = false;

volatile enum {
  Lin_Send_State_NA = 0,
  Lin_Send_State_Break = 1,
  Lin_Send_State_0x55_PID,
  Lin_Send_State_Data_send
  
} Lin_Send_State;


volatile enum {
  Lin_Recv_State_NA = 0,
  Lin_Recv_State_Break = 1,
  Lin_Recv_State_0x55,
  Lin_Recv_State_PID,
  Lin_Recv_State_Data,
  Lin_Recv_State_checkSum
  
} Lin_Recv_State;
uint8_t Lin_data_recv_idx = 0;
uint8_t Lin_data_recv_buf[15];


uint8_t Lin_ID_to_PID(uint8_t ID);

int Lin_SendBreak(void)
{
//  if (lin_data_sending || uart_data_sending) { return -1;}
  lin_data_sending = true;
  uart_data_sending = true;
  Lin_Send_State = Lin_Send_State_Break;
  
  Lin_Recv_State = Lin_Recv_State_Break;
  nrf_uart_baudrate_set(NRF_UART0, NRF_UART_BAUDRATE_9600);
  app_uart_flush();
  app_uart_put(0);
  return 0;
}

int Lin_Send_0x55_PID(void)
{
  if (uart_data_sending){ return -1; }
  uart_data_sending = true;
  Lin_Send_State = Lin_Send_State_0x55_PID;
  nrf_uart_baudrate_set(NRF_UART0, NRF_UART_BAUDRATE_19200);
  app_uart_flush();
  app_uart_put(0x55);
  app_uart_put(Lin_ID_to_PID(Lin_execute_current_ID));
  return 0;
}

void Lin_SendData(const uint8_t a[], uint8_t n)
{
  uint8_t i;
  app_uart_flush();
  for (i = 0; i < n; i++) 
  {
    app_uart_put(a[i]);
  }
}

uint8_t Lin_ID_to_PID(uint8_t ID)
{
    uint8_t p0 = 0x00, p1=0x00, p1_1 = 0x00, PID;

    PID = ID & 0x3f;
    
    p0 = (PID & 0x01)^((PID&0x02)>>1)^((PID&0x04)>>2)^((PID&0x10)>>4);
    p0 = p0 & 0x01;
    p1_1=((PID&0x02)>>1)^((PID&0x08)>>3)^((PID&0x10)>>4)^((PID&0x20)>>5);
    p1=(~p1_1)&0x01;
	
    PID = (((p0 << 6) | (p1 << 7)) & (~0x3f)) | PID;
  
    return PID;
}

uint8_t Lin_ID_to_len(uint8_t ID)
{
  uint8_t len = 8; 
  uint8_t i = (ID >> 4) & 0x03; 
  
  if (i == 0 || i == 1) {
    len = 2;
  } else if (i == 2) {
    len = 4;
  } else if (i == 3) {
    len = 8;
  }
  
  return len;
}

void send_string(uint8_t * p_string, uint16_t length);


void Lin_data_ready(uint8_t byte)
{
    static uint8_t lin_rec_id, lin_data_len;
    uint8_t i = 0;

    switch (Lin_Recv_State)
    {
      case Lin_Recv_State_NA:
        break;
      case Lin_Recv_State_Break:
        if (byte == 0) {
          Lin_data_recv_idx = 0;
          Lin_data_recv_buf[Lin_data_recv_idx] = byte;
          Lin_data_recv_idx++;
          Lin_Recv_State = Lin_Recv_State_0x55;
        }
        break;
      case Lin_Recv_State_0x55:
        if (byte == 0x55) {
          Lin_data_recv_buf[Lin_data_recv_idx] = byte;
          Lin_data_recv_idx++;
          Lin_Recv_State = Lin_Recv_State_PID;
        } else {
          Lin_Recv_State = Lin_Recv_State_Break;
        }
        break;
      case Lin_Recv_State_PID:
        if (Lin_CheckPID(byte) == 0) {
          lin_rec_id = byte & 0x3f;
          lin_data_len = Lin_ID_to_len(lin_rec_id);
          Lin_data_recv_buf[Lin_data_recv_idx] = byte;
          Lin_data_recv_idx++;
          Lin_Recv_State = Lin_Recv_State_Data;
        } else {
          Lin_Recv_State = Lin_Recv_State_Break;
        }
        break;
      case Lin_Recv_State_Data:
        
        Lin_data_recv_buf[Lin_data_recv_idx++] = byte;
        if (Lin_data_recv_idx >= 3 + lin_data_len) {
          Lin_Recv_State = Lin_Recv_State_checkSum;
//          nrf_gpio_pin_write(19, 1);
          if (is_connected) {
            nrf_gpio_pin_write(30, 1);
            nrf_gpio_pin_write(20, 1);
          }
        }
        break;
      case Lin_Recv_State_checkSum:
//        nrf_gpio_pin_write(19, 0);
        if (is_connected) {
            nrf_gpio_pin_write(30, 0);
            nrf_gpio_pin_write(20, 0);
        }
        Lin_data_recv_buf[Lin_data_recv_idx++] = byte;
        if (Lin_Check_Sum(Lin_data_recv_buf + 3, lin_data_len) == byte) {
            
            for (i = 0; i < lin_data_len && i < 8; i++) {
                lin_data[lin_rec_id].data[i] =  Lin_data_recv_buf[3 + i];
            }
            lin_data[lin_rec_id].update = 1;
            
            send_string(Lin_data_recv_buf + 1, 3 + lin_data_len);
            
        } else {
            Lin_Recv_State = Lin_Recv_State_Break;
        }
        break;
    }
}

void Lin_data_tx_done(void)
{
  uint8_t ID = 0, i = 0, lin_data_len;
  
  uart_data_sending = false;
  switch (Lin_Send_State)
  {
    case Lin_Send_State_NA:
      break;
    case Lin_Send_State_Break:
      Lin_Send_0x55_PID();
    
      Lin_data_send_idx = 0;
      Lin_data_send_buf[Lin_data_send_idx++] = 0;
    
      break;
    case Lin_Send_State_0x55_PID:
      ID = Lin_execute_current_ID & 0x3f;
      lin_data_len = Lin_ID_to_len(ID);

      if (lin_data[ID].enable && lin_data[ID].press) {
        
          for (i = 0; i < lin_data_len; i++)
          {
              Lin_data_send_buf[3 + i] = lin_data[ID].data[i];
          }
          Lin_data_send_buf[3 + lin_data_len] = Lin_Check_Sum(Lin_data_send_buf + 3, lin_data_len);
          
          Lin_SendData(Lin_data_send_buf + 3, lin_data_len + 1);

          lin_data[ID].send_one_time_active --;
          
          Lin_Send_State = Lin_Send_State_Data_send;
      } else {
          Lin_Send_State = Lin_Send_State_NA;
          lin_data_sending = false;
      }
        
      break;
    case Lin_Send_State_Data_send:
      Lin_Send_State = Lin_Send_State_NA;
      lin_data_sending = false;
      break;
  }
}

//校验PID
uint8_t Lin_CheckPID(uint8_t PID)
{
	uint8_t p0=0x00,p1=0x00,p1_1=0x00;
	p0=(PID&0x01)^((PID&0x02)>>1)^((PID&0x04)>>2)^((PID&0x10)>>4);
	p1_1=((PID&0x02)>>1)^((PID&0x08)>>3)^((PID&0x10)>>4)^((PID&0x20)>>5);
	p1=(~p1_1)&0x01;
	if((p0==((PID&0x40)>>6)) && (p1==((PID&0x80)>>7)))
	{
		return 0;//PID段校验正确
	}
	else
	{
		return 1;//PID段校验错误
	}
}
//Lin数据和校验
uint8_t Lin_Check_Sum(uint8_t* data, uint8_t n)
{
  uint16_t sum=0;
  
  while(n--)
  {
    sum += *data++;
    sum = (sum & 0xFF) + ((sum >> 8) & 0xFF);
  }
  
  return ~sum & 0xFF;
}

void Lin_data_init(void)
{
    int i, j;
    
    memset(lin_data, 0, sizeof(lin_data));
    
    // 本机发布 偶尔发一次
    for (i = 0x31; i <= 0x31; i++)
    {
        lin_data[i].enable   = 1;
        lin_data[i].press    = 1;
        lin_data[i].receive  = 0;
        lin_data[i].send_one_time_enable = 1;
    }
    
    // 本机接收
    for (i = 0x32; i <= 0x35; i++)
    {
        lin_data[i].enable   = 1;
        lin_data[i].press    = 0;
        lin_data[i].receive  = 1;
        
        for (j = 0; j < 8; j++)
        {
            lin_data[i].data[j] = 0xff;
        }
    }
    
    // 本机发布 偶尔发一次
    for (i = 0x38; i <= 0x38; i++)
    {
        lin_data[i].enable   = 1;
        lin_data[i].press    = 1;
        lin_data[i].receive  = 0;
        lin_data[i].send_one_time_enable = 1;
    }
    
    // 本机接收
    for (i = 0x39; i <= 0x39; i++)
    {
        lin_data[i].enable   = 1;
        lin_data[i].press    = 0;
        lin_data[i].receive  = 1;
        
        for (j = 0; j < 8; j++)
        {
        //    lin_data[i].data[j] = 0xff;
        }
    }
    /* */
    
//    // 0x3c是主机发布的ID  诊断帧 主机请求帧ID=0x3c 应答部分的发布节点为主机节点  
//    for (i = 0x3c; i <= 0x3c; i++)
//    {
//        lin_data[i].enable   = 0;
//        lin_data[i].press    = (IS_SLAVE ? 0 : 1);
//        lin_data[i].receive  = (IS_SLAVE ? 1 : 0);
//        lin_data[i].send_one_time_enable = 0;
//        
//        lin_data[i].data[0] = (IS_SLAVE ? 0xff : 0xaa);
//        lin_data[i].data[1] = (IS_SLAVE ? 0xff : 0x55);
//    }
//    // 0x3d是从机发布的ID  诊断帧 从机应答帧ID=0x3d  
//    for (i = 0x3d; i <= 0x3d; i++)
//    {
//        lin_data[i].enable   = 0;
//        lin_data[i].press    = (IS_SLAVE ? 1 : 0);
//        lin_data[i].receive  = (IS_SLAVE ? 0 : 1);
//        
//        lin_data[i].data[0] = (IS_SLAVE ? 0xaa : 0xff);
//        lin_data[i].data[1] = (IS_SLAVE ? 0x55 : 0xff);
//    }
    
    
    Lin_init_flag |= Lin_FLAG_data_init;
}

// 主机发布一次
int Lin_ID_data_press(const uint8_t ID, const uint8_t data[8])
{
    int j = 0;
    
    if (!(lin_data[ID & 0x3f].enable && lin_data[ID & 0x3f].press))
    {
        return 1;
    }
    if (data != NULL)
    {
        for (j = 0; j < 8; j++)
        {
            lin_data[ID & 0x3f].data[j] = data[j];
        }
    }
    
    // 偶发帧
    if (lin_data[ID & 0x3f].send_one_time_enable)
    {
        // 设置发送一次的标志
        lin_data[ID & 0x3f].send_one_time_active = 1;
    }
    return 0;
}

static int is_ID_execute(uint8_t id)
{
    id = id & 0x3f;
    
    if (!lin_data[id].enable)
    {
        return 0;
    }
    
    if (lin_data[id].receive && (!lin_data[id].press))    // 接收类型ID
    {
        return 1;
    }
    else if ((!lin_data[id].receive) && lin_data[id].press)  // 发布类型ID
    {
        // 偶发
        if (lin_data[id].send_one_time_enable)
        {
            // 发送
            if (lin_data[id].send_one_time_active > 0)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 1;
        }
    }
    else if ((!lin_data[id].receive) && (!lin_data[id].press))  // 不发布不接受ID
    {
        return 0;
    }
    else
    {
        return 0;
    }
}

// 主机定时调用
void Lin_master_go(void)
{
    static uint8_t id_last = 0, id = 0x40;
    uint8_t cnt = 0;
    
    if (
        (Lin_init_flag & Lin_FLAG_data_init) 
    != 
         Lin_FLAG_data_init
    )
    {
        return;
    }
    
    id_last = id;
    
    do {
        id++;
        if (id > 0x3d)
        {
            id = 0;
        }
        cnt ++;
    } while ((! is_ID_execute(id)) && cnt <= LIN_DATA_LEN);
    
    if (cnt <= LIN_DATA_LEN)
    {
        Lin_execute_current_ID = id;
        if ( Lin_SendBreak() != 0)
        {
            id = id_last;
        }
    }
}
