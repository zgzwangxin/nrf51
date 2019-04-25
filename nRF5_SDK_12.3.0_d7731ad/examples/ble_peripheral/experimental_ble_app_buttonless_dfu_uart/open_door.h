/* 
    开门状态管理
    open_door.h
 */
 
 #ifndef OPEN_DOOR_STATE_MANAGE_H__
 #define OPEN_DOOR_STATE_MANAGE_H__
 
 
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OPEN_DOOR_CMD_NAN,
    OPEN_DOOR_CMD_OPEN,
    
} open_door_cmd_state_t;

typedef enum {
    
    OPEN_DOOR_STATE_CLOSE,
    OPEN_DOOR_STATE_Start,
    OPEN_DOOR_STATE_OPEN_OUT,
    OPEN_DOOR_STATE_NO_OUT,
    OPEN_DOOR_STATE_CLOSE_OUT,
    
} open_door_state_t;
    

typedef struct {
    
    uint32_t    open_out_time_cnt;
    uint32_t    open_out_time_N;
    
    uint32_t    no_out_time_cnt;
    uint32_t    no_out_time_N;
    
    uint32_t    close_out_time_cnt;
    uint32_t    close_out_time_N;
    
    
} open_door_data_t;

typedef struct {
    
    open_door_cmd_state_t   cmd_state;
    open_door_state_t       door_state;
    
    open_door_data_t        door_data;
    
} open_door_manage_t;

extern  open_door_manage_t  g_open_door_manage;

int open_door_manage_init(open_door_manage_t * popen_door_manage);

int open_door_manage_check(open_door_manage_t * popen_door_manage);
 
#ifdef __cplusplus
}
#endif

 
 #endif // OPEN_DOOR_STATE_MANAGE_H__
 
