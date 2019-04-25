/**
    指纹模块
*/


#ifndef FINGER_MARK_H__
#define FINGER_MARK_H__


#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#define FENGER_TOUCH_PIN_NUMBER         03
#define FENGER_TOUCH_ON_PIN_NUMBER      04
#define FENGER_TOUCH_ON_ACTIVE_LEVEL    0

#define FINGER_MARK_MAX_CMD_LEN     50         /* 指纹指令的最大长度 */
#define FINGER_MARK_MAX_DATA_LEN    100         /* 指纹数据的最大长度 */
    
/*
    返回错误码
 */
#define FINGER_MARK_SUCCESS     0
#define FINGER_MARK_ERROR       1

// 指纹模块命令
typedef struct {
    uint16_t    len;
    uint8_t     data[FINGER_MARK_MAX_CMD_LEN];
} finger_mark_cmd_t;
    
extern finger_mark_cmd_t   g_finger_mark_cmd;

// 包类型
typedef enum {
    PACK_CMD                =   1,
    PACK_DATA_WITH_NEXT     =   2,
    PACK_DATA_ENDING        =   8,
    PACK_CMD_REPLY          =   7,
} finger_mark_pack_type_t;

// 数据包
typedef struct {
    bool                        is_pack_valid;
    finger_mark_pack_type_t     pack_type;
    uint16_t                    pack_len;
    uint8_t                     pack_data[FINGER_MARK_MAX_DATA_LEN];
    
} finger_mark_pack_t;

typedef enum {
    STATE_WAIT_PACK_HEAD,
    STATE_WAIT_ADDR,
    STATE_WAIT_PACK_TYPE,
    STATE_WAIT_PACK_LEN,
    STATE_WAIT_PACK_DATA,
    STATE_WAIT_CHECK_SUM,
    
} receive_state_t;

// 指纹模块接收数据状态机
typedef struct {
    
    finger_mark_pack_t      pack;
    
    receive_state_t         receive_state;
    uint16_t                sub_state_receive_char_len;
    uint16_t                data_state_receive_char_len;
    uint16_t                check_sum_calc;
    uint16_t                check_sum_receive;
    finger_mark_pack_type_t receive_pack_type;
    uint8_t                 sub_state_receive_data[FINGER_MARK_MAX_DATA_LEN];
 
} finger_mark_receive_state_machine_t;

extern finger_mark_receive_state_machine_t  g_finger_mark_receive_state_machine;

typedef struct {
    
    bool is_finger_touch_pushed;
    bool is_finger_touch_pushed_last;
    bool is_finger_on;
    bool is_finger_off;
    
    bool is_long_push;
    int  long_push_cnt;
    
} finger_mark_touch_state_t;

typedef enum {
    USER_CMD_NONE,
    USER_CMD_RECOGNITION_FINGER_MARK,   // 指纹识别命令
    USER_CMD_ENROLL_FINGER_MARK,        // 指纹注册命令
    USER_CMD_ENROLL_FINGER_MARK_CANCEL, // 取消
    USER_CMD_CLEAR_ALL_FINGER_MARK,     // 清空指纹库
    
} user_cmd_type_t;

//
typedef struct {
    user_cmd_type_t     user_cmd_type;
    
} user_cmd_t;

typedef enum {
    
    MANAGE_STATE_IDLE,
    MANAGE_STATE_ENROLL_FINGER_MARK,
    MANAGE_STATE_RECOGNITION_FINGER_MARK,
    MANAGE_STATE_CLEAR_ALL_FINGER_MARK,
    MANAGE_STATE_READ_INDEX_TABLE,
    MANAGE_STATE_POWER,
    
} manage_state_t;

typedef enum {

    MANAGE_STATE_ENROLL_FINGER_MARK_START,                      // 开始注册
    
    MANAGE_STATE_ENROLL_FINGER_MARK_READ_INDEX_TABLE,
    
    MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Send,        // 发送获取图像指令
    MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Wait_Reply,  // 应答
    MANAGE_STATE_ENROLL_FINGER_MARK_GenChar_Send,               // 发送生成特征指令
    MANAGE_STATE_ENROLL_FINGER_MARK_GenChar_Wait_Reply,         // 应答
    
    MANAGE_STATE_ENROLL_FINGER_MARK_RegModel_Send,              // 发送生成特征指令
    MANAGE_STATE_ENROLL_FINGER_MARK_RegModel_Wait_Reply,        // 应答
    MANAGE_STATE_ENROLL_FINGER_MARK_StoreChar_Send,             // 发送存储模板
    MANAGE_STATE_ENROLL_FINGER_MARK_StoreChar_Wait_Reply,       // 应答
    
    MANAGE_STATE_ENROLL_FINGER_MARK_SECCUSS,                    // 注册成功
    MANAGE_STATE_ENROLL_FINGER_MARK_FAILURE,                    // 注册失败
    
} manage_state_enroll_t;

typedef struct {
    uint32_t  enroll_cnt;
    uint32_t  enroll_cnt_last;
    uint32_t  enroll_N;
    
    bool      is_read_index_table;
    
    uint16_t  store_page_ID;
    
} data_enroll_t;

typedef enum {
    
    MANAGE_STATE_RECOGNITION_START,                     // 开始识别
    
    MANAGE_STATE_RECOGNITION_GetImage_Send,             // 发送获取图像指令
    MANAGE_STATE_RECOGNITION_GetImage_Wait_Reply,       // 应答
    MANAGE_STATE_RECOGNITION_GenChar_Send,              // 发送生成特征指令
    MANAGE_STATE_RECOGNITION_GenChar_Wait_Reply,        // 应答
    
    MANAGE_STATE_RECOGNITION_Search_Send,               // 搜索指纹指令
    MANAGE_STATE_RECOGNITION_Search_Wait_Reply,         // 应答
    
    MANAGE_STATE_RECOGNITION_Search_SUCCESS,            // 搜索成功
    MANAGE_STATE_RECOGNITION_Search_FAILURE,            // 搜索成功
    
} manage_state_recognition_t;

typedef struct {
    uint8_t     BufferID;
    uint16_t    start_page;
    uint16_t    page_num;
    uint16_t    ret_page_ID;
    uint16_t    ret_match_score;
    
} data_recognition_t;


typedef enum {
    
    MANAGE_STATE_CLEAR_START,                   // 开始清空
    
    MANAGE_STATE_CLEAR_Empty_Send,              // 发送清空指纹指令
    MANAGE_STATE_CLEAR_Empty_Wait_Reply,        // 应答
    
    MANAGE_STATE_CLEAR_SUCCESS,                 // 清空指纹成功
    MANAGE_STATE_CLEAR_FAILURE,                 // 清空指纹成功
    
} manage_state_clear_all_finger_mark_t;


typedef struct {
    bool     is_empty_accomplish;
    
} data_clear_all_finger_mark_t;

typedef enum {
    
    MANAGE_STATE_READ_INDEX_TABLE_START,
    
    
    MANAGE_STATE_READ_INDEX_TABLE_Send,
    
    MANAGE_STATE_READ_INDEX_TABLE_Wait_Reply,
    
    MANAGE_STATE_READ_INDEX_TABLE_SUCCESS,
    MANAGE_STATE_READ_INDEX_TABLE_FAILURE,
    
} manage_state_read_index_table_t;

typedef struct {
    
    bool        is_table_read;
    
    uint32_t    read_timeout_cnt;
    uint32_t    read_timeout_N;
    
    uint8_t     read_index_table_ID;
    uint8_t     index_value[4][32];
    uint16_t    valid_templete_num[4];
    
    manage_state_t  next_state;
    
} data_read_index_table_t;

typedef enum {
    
    MANAGE_STATE_POWER_WAKE_UP_START,
    
    MANAGE_STATE_POWER_WAKE_UP_WAIT,
    
    MANAGE_STATE_POWER_SLEEP_START,
    
    MANAGE_STATE_POWER_SLEEPING,
    
} manage_state_power_t;

typedef struct {
    
    uint32_t    wait_time_cnt;
    uint32_t    wait_time_N;
    
    uint32_t    sleep_timeout_cnt;
    uint32_t    sleep_timeout_N;
    
    manage_state_t  next_state;
    
} data_power_t;


// 指纹模块管理状态机
typedef struct {
    
    finger_mark_touch_state_t   touch_state;
    
    user_cmd_t                  user_cmd;
    
    manage_state_t              manage_state;
    
    manage_state_enroll_t       state_enroll;
    data_enroll_t               data_enroll;
    
    manage_state_recognition_t  state_recognition;
    data_recognition_t          data_recognition;
    
    manage_state_clear_all_finger_mark_t    state_clear_all_finger;
    data_clear_all_finger_mark_t            data_clear_all_finger;
    
    manage_state_read_index_table_t         state_read_index_table;
    data_read_index_table_t                 data_read_index_table;
    
    manage_state_power_t            state_power;
    data_power_t                    data_power;
    
} finger_mark_manage_state_machine_t;
    
extern finger_mark_manage_state_machine_t  g_finger_mark_manage_state_machine;

/*
    发送命令
 */
int finger_mark_send_cmd(const finger_mark_cmd_t * pfinger_mark_cmd);



/*
    0x02 根据原始图像生成挃纹特征存于特征文件缓冲区
 */
int finger_mark_gen_cmd_GenChar(finger_mark_cmd_t * pfinger_mark_cmd, uint8_t BufferID);

/*
    0x04 以特征文件缓冲区中的特征文件搜索整个或部分指纹库
 */
int finger_mark_gen_cmd_Search(
    finger_mark_cmd_t *     pfinger_mark_cmd, 
    uint8_t                 BufferID,
    uint16_t                StartPage,
    uint16_t                PageNum);

/*
    0x05 将特征文件合并生成模板存于特征文件
 */
int finger_mark_gen_cmd_RegModel(finger_mark_cmd_t * pfinger_mark_cmd);


/*
    0x06 将特征缓冲区中的文件储存到 flash 挃纹库中
 */
int finger_mark_gen_cmd_StoreChar(
     finger_mark_cmd_t * pfinger_mark_cmd, 
     uint8_t             BufferID, 
     uint16_t            PageID);

/*
    0x0c 删除 flash 挃纹库中的一个特征文件
 */
int finger_mark_gen_cmd_DeletChar(
     finger_mark_cmd_t * pfinger_mark_cmd,  
     uint16_t            PageID, 
     uint16_t            PageNum);

/*
    0x0d 清空 flash 指纹库
 */
int finger_mark_gen_cmd_Empty(finger_mark_cmd_t * pfinger_mark_cmd);

/*
    0x0f 读系统基本参数
 */
int finger_mark_gen_cmd_ReadSysPara(finger_mark_cmd_t * pfinger_mark_cmd);

/*
    0x14采样随机数
 */
int finger_mark_gen_cmd_GetRandomCode(finger_mark_cmd_t * pfinger_mark_cmd);

/*
    0x1d 读有效模板个数
 */
int finger_mark_gen_cmd_ValidTempleteNum(finger_mark_cmd_t * pfinger_mark_cmd);

/*
    0x1f 读索引表
 */
int finger_mark_gen_cmd_ReadIndexTable(finger_mark_cmd_t * pfinger_mark_cmd, uint8_t index_table_page_ID);

/*
    0x29注册用获取图像
 */
int finger_mark_gen_cmd_GetEnrollImage(finger_mark_cmd_t * pfinger_mark_cmd);

/*
    
 */
int FingerMarkDataProcess_init(
    finger_mark_receive_state_machine_t * preceive_state_machine);



/*
    解析从指纹模块收到的数据包
 */
int FingerMarkDataProcess(
    finger_mark_receive_state_machine_t * preceive_state_machine,
    uint8_t             c);

/*
    管理状态初始化
 */
int finger_mark_manage_state_machine_init(
        finger_mark_manage_state_machine_t *pfinger_mark_manage_state_machine);

/*
    检查触摸状态
 */
int finger_mark_check_touch_state(
        finger_mark_manage_state_machine_t *pmanage_state);

/*
    检查状态
 */
int finger_mark_check_manage_state(
        finger_mark_manage_state_machine_t *pmanage_state);



#ifdef __cplusplus
}
#endif

#endif // FINGER_MARK_H__
