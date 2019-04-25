/**
    ָ��ģ��
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

#define FINGER_MARK_MAX_CMD_LEN     50         /* ָ��ָ�����󳤶� */
#define FINGER_MARK_MAX_DATA_LEN    100         /* ָ�����ݵ���󳤶� */
    
/*
    ���ش�����
 */
#define FINGER_MARK_SUCCESS     0
#define FINGER_MARK_ERROR       1

// ָ��ģ������
typedef struct {
    uint16_t    len;
    uint8_t     data[FINGER_MARK_MAX_CMD_LEN];
} finger_mark_cmd_t;
    
extern finger_mark_cmd_t   g_finger_mark_cmd;

// ������
typedef enum {
    PACK_CMD                =   1,
    PACK_DATA_WITH_NEXT     =   2,
    PACK_DATA_ENDING        =   8,
    PACK_CMD_REPLY          =   7,
} finger_mark_pack_type_t;

// ���ݰ�
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

// ָ��ģ���������״̬��
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
    USER_CMD_RECOGNITION_FINGER_MARK,   // ָ��ʶ������
    USER_CMD_ENROLL_FINGER_MARK,        // ָ��ע������
    USER_CMD_ENROLL_FINGER_MARK_CANCEL, // ȡ��
    USER_CMD_CLEAR_ALL_FINGER_MARK,     // ���ָ�ƿ�
    
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

    MANAGE_STATE_ENROLL_FINGER_MARK_START,                      // ��ʼע��
    
    MANAGE_STATE_ENROLL_FINGER_MARK_READ_INDEX_TABLE,
    
    MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Send,        // ���ͻ�ȡͼ��ָ��
    MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Wait_Reply,  // Ӧ��
    MANAGE_STATE_ENROLL_FINGER_MARK_GenChar_Send,               // ������������ָ��
    MANAGE_STATE_ENROLL_FINGER_MARK_GenChar_Wait_Reply,         // Ӧ��
    
    MANAGE_STATE_ENROLL_FINGER_MARK_RegModel_Send,              // ������������ָ��
    MANAGE_STATE_ENROLL_FINGER_MARK_RegModel_Wait_Reply,        // Ӧ��
    MANAGE_STATE_ENROLL_FINGER_MARK_StoreChar_Send,             // ���ʹ洢ģ��
    MANAGE_STATE_ENROLL_FINGER_MARK_StoreChar_Wait_Reply,       // Ӧ��
    
    MANAGE_STATE_ENROLL_FINGER_MARK_SECCUSS,                    // ע��ɹ�
    MANAGE_STATE_ENROLL_FINGER_MARK_FAILURE,                    // ע��ʧ��
    
} manage_state_enroll_t;

typedef struct {
    uint32_t  enroll_cnt;
    uint32_t  enroll_cnt_last;
    uint32_t  enroll_N;
    
    bool      is_read_index_table;
    
    uint16_t  store_page_ID;
    
} data_enroll_t;

typedef enum {
    
    MANAGE_STATE_RECOGNITION_START,                     // ��ʼʶ��
    
    MANAGE_STATE_RECOGNITION_GetImage_Send,             // ���ͻ�ȡͼ��ָ��
    MANAGE_STATE_RECOGNITION_GetImage_Wait_Reply,       // Ӧ��
    MANAGE_STATE_RECOGNITION_GenChar_Send,              // ������������ָ��
    MANAGE_STATE_RECOGNITION_GenChar_Wait_Reply,        // Ӧ��
    
    MANAGE_STATE_RECOGNITION_Search_Send,               // ����ָ��ָ��
    MANAGE_STATE_RECOGNITION_Search_Wait_Reply,         // Ӧ��
    
    MANAGE_STATE_RECOGNITION_Search_SUCCESS,            // �����ɹ�
    MANAGE_STATE_RECOGNITION_Search_FAILURE,            // �����ɹ�
    
} manage_state_recognition_t;

typedef struct {
    uint8_t     BufferID;
    uint16_t    start_page;
    uint16_t    page_num;
    uint16_t    ret_page_ID;
    uint16_t    ret_match_score;
    
} data_recognition_t;


typedef enum {
    
    MANAGE_STATE_CLEAR_START,                   // ��ʼ���
    
    MANAGE_STATE_CLEAR_Empty_Send,              // �������ָ��ָ��
    MANAGE_STATE_CLEAR_Empty_Wait_Reply,        // Ӧ��
    
    MANAGE_STATE_CLEAR_SUCCESS,                 // ���ָ�Ƴɹ�
    MANAGE_STATE_CLEAR_FAILURE,                 // ���ָ�Ƴɹ�
    
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


// ָ��ģ�����״̬��
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
    ��������
 */
int finger_mark_send_cmd(const finger_mark_cmd_t * pfinger_mark_cmd);



/*
    0x02 ����ԭʼͼ�����ɒ����������������ļ�������
 */
int finger_mark_gen_cmd_GenChar(finger_mark_cmd_t * pfinger_mark_cmd, uint8_t BufferID);

/*
    0x04 �������ļ��������е������ļ����������򲿷�ָ�ƿ�
 */
int finger_mark_gen_cmd_Search(
    finger_mark_cmd_t *     pfinger_mark_cmd, 
    uint8_t                 BufferID,
    uint16_t                StartPage,
    uint16_t                PageNum);

/*
    0x05 �������ļ��ϲ�����ģ����������ļ�
 */
int finger_mark_gen_cmd_RegModel(finger_mark_cmd_t * pfinger_mark_cmd);


/*
    0x06 �������������е��ļ����浽 flash ���ƿ���
 */
int finger_mark_gen_cmd_StoreChar(
     finger_mark_cmd_t * pfinger_mark_cmd, 
     uint8_t             BufferID, 
     uint16_t            PageID);

/*
    0x0c ɾ�� flash ���ƿ��е�һ�������ļ�
 */
int finger_mark_gen_cmd_DeletChar(
     finger_mark_cmd_t * pfinger_mark_cmd,  
     uint16_t            PageID, 
     uint16_t            PageNum);

/*
    0x0d ��� flash ָ�ƿ�
 */
int finger_mark_gen_cmd_Empty(finger_mark_cmd_t * pfinger_mark_cmd);

/*
    0x0f ��ϵͳ��������
 */
int finger_mark_gen_cmd_ReadSysPara(finger_mark_cmd_t * pfinger_mark_cmd);

/*
    0x14���������
 */
int finger_mark_gen_cmd_GetRandomCode(finger_mark_cmd_t * pfinger_mark_cmd);

/*
    0x1d ����Чģ�����
 */
int finger_mark_gen_cmd_ValidTempleteNum(finger_mark_cmd_t * pfinger_mark_cmd);

/*
    0x1f ��������
 */
int finger_mark_gen_cmd_ReadIndexTable(finger_mark_cmd_t * pfinger_mark_cmd, uint8_t index_table_page_ID);

/*
    0x29ע���û�ȡͼ��
 */
int finger_mark_gen_cmd_GetEnrollImage(finger_mark_cmd_t * pfinger_mark_cmd);

/*
    
 */
int FingerMarkDataProcess_init(
    finger_mark_receive_state_machine_t * preceive_state_machine);



/*
    ������ָ��ģ���յ������ݰ�
 */
int FingerMarkDataProcess(
    finger_mark_receive_state_machine_t * preceive_state_machine,
    uint8_t             c);

/*
    ����״̬��ʼ��
 */
int finger_mark_manage_state_machine_init(
        finger_mark_manage_state_machine_t *pfinger_mark_manage_state_machine);

/*
    ��鴥��״̬
 */
int finger_mark_check_touch_state(
        finger_mark_manage_state_machine_t *pmanage_state);

/*
    ���״̬
 */
int finger_mark_check_manage_state(
        finger_mark_manage_state_machine_t *pmanage_state);



#ifdef __cplusplus
}
#endif

#endif // FINGER_MARK_H__
