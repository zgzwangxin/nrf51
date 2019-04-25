/* 
    开门状态管理
    open_door.c
 */
 
#include "open_door.h"
 
#include "nrf_gpio.h"
 
#include "global.h"
 
open_door_manage_t  g_open_door_manage;

int open_door_manage_init(open_door_manage_t * popen_door_manage)
{
    if (popen_door_manage == NULL)
    {
        return 1;
    }
    
    popen_door_manage->cmd_state    = OPEN_DOOR_CMD_NAN;
    popen_door_manage->door_state   = OPEN_DOOR_STATE_CLOSE;
    
    return 0;
}

 
int open_door_manage_check(open_door_manage_t * popen_door_manage)
{
    if (popen_door_manage == NULL)
    {
        return 1;
    }
    
    if (popen_door_manage->cmd_state == OPEN_DOOR_CMD_OPEN)
    {
        popen_door_manage->door_state = OPEN_DOOR_STATE_Start;
        
        popen_door_manage->cmd_state = OPEN_DOOR_CMD_NAN;
    }
    
    switch (popen_door_manage->door_state)
    {
        case OPEN_DOOR_STATE_Start:
            // 10 ms 一次
            popen_door_manage->door_data.open_out_time_cnt  =   0;
            popen_door_manage->door_data.open_out_time_N    =   MS_TO_CNT(500, TIMER1_CIRCLE_MS);
        
            popen_door_manage->door_data.no_out_time_cnt  =   0;
            popen_door_manage->door_data.no_out_time_N    =   MS_TO_CNT(5000, TIMER1_CIRCLE_MS);
        
            popen_door_manage->door_data.close_out_time_cnt  =   0;
            popen_door_manage->door_data.close_out_time_N    =   MS_TO_CNT(500, TIMER1_CIRCLE_MS);
            
            // 输出开
            nrf_gpio_pin_write(28, 0);
            nrf_gpio_pin_write(29, 1);
        
                
            nrf_gpio_pin_write(19, 1);
            nrf_gpio_pin_write(20, 0);
            

            popen_door_manage->door_state   =   OPEN_DOOR_STATE_OPEN_OUT;
        
            break;
        
        case OPEN_DOOR_STATE_OPEN_OUT:
            popen_door_manage->door_data.open_out_time_cnt++;
            
            if (popen_door_manage->door_data.open_out_time_cnt 
                >= 
                popen_door_manage->door_data.open_out_time_N)
            {
                
                // 输出无
                nrf_gpio_pin_write(28, 0);
                nrf_gpio_pin_write(29, 0);
                
                nrf_gpio_pin_write(19, 0);
                nrf_gpio_pin_write(20, 0);
                
                popen_door_manage->door_state   =   OPEN_DOOR_STATE_NO_OUT;
            }
            
            break;
            
        case OPEN_DOOR_STATE_NO_OUT:
            popen_door_manage->door_data.no_out_time_cnt++;
            
            if (popen_door_manage->door_data.no_out_time_cnt 
                >= 
                popen_door_manage->door_data.no_out_time_N)
            {
                
                // 输出关
                nrf_gpio_pin_write(28, 1);
                nrf_gpio_pin_write(29, 0);
                
                nrf_gpio_pin_write(19, 0);
                nrf_gpio_pin_write(20, 1);
                
                popen_door_manage->door_state   =   OPEN_DOOR_STATE_CLOSE_OUT;
            }
            
            break;
            
        case OPEN_DOOR_STATE_CLOSE_OUT:
            
            popen_door_manage->door_data.close_out_time_cnt++;
        
            if (popen_door_manage->door_data.close_out_time_cnt
                >=
                popen_door_manage->door_data.close_out_time_N)
            {
                
                // 不输出
                nrf_gpio_pin_write(28, 0);
                nrf_gpio_pin_write(29, 0);
                
                
                nrf_gpio_pin_write(19, 0);
                nrf_gpio_pin_write(20, 0);
            
                
                popen_door_manage->door_state   =   OPEN_DOOR_STATE_CLOSE;
            }
            
            break;
        
        case OPEN_DOOR_STATE_CLOSE:
            break;
        
        default:
            break;
    }
    
    
    return 0;
}

