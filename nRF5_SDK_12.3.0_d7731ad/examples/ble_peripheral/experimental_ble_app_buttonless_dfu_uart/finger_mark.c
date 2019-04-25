
#include "finger_mark.h"

#include "open_door.h"
 
#include "app_uart.h"
#include "ble_nus.h"

#include "boards.h"

#include <string.h>
#include <stdio.h>

#include "global.h"

void send_string(uint8_t * p_string, uint16_t length);
void send_string_tx(uint8_t * p_string, uint16_t length);

finger_mark_cmd_t   g_finger_mark_cmd;

finger_mark_receive_state_machine_t  g_finger_mark_receive_state_machine;

finger_mark_manage_state_machine_t  g_finger_mark_manage_state_machine;

#define STRING_LEN  18
char string[STRING_LEN];

void open_cmd_start(void);

void uart_init(void);

/*
    ��������
 */
int finger_mark_send_cmd(const finger_mark_cmd_t * pfinger_mark_cmd)
{
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    if (pfinger_mark_cmd->len > FINGER_MARK_MAX_CMD_LEN)
    {
        return FINGER_MARK_ERROR;
    }
    
    for (uint32_t i = 0; i < pfinger_mark_cmd->len; i++)
    {
        while (app_uart_put(pfinger_mark_cmd->data[i]) != NRF_SUCCESS);
    }
    return FINGER_MARK_SUCCESS;
}

/*
    0x01 �Ӵ������϶���ͼ�����ͼ�񻺳���
 */
int finger_mark_gen_cmd_GetImage(finger_mark_cmd_t * pfinger_mark_cmd)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x03;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x02 ����ԭʼͼ�����ɒ����������������ļ�������
 */
int finger_mark_gen_cmd_GenChar(finger_mark_cmd_t * pfinger_mark_cmd, uint8_t BufferID)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    if (BufferID == 0 || BufferID > 20)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x04;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x02;
    
    // ��������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = BufferID;
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x04 �������ļ��������е������ļ����������򲿷�ָ�ƿ�
 */
int finger_mark_gen_cmd_Search(
    finger_mark_cmd_t *     pfinger_mark_cmd, 
    uint8_t                 BufferID,
    uint16_t                StartPage,
    uint16_t                PageNum)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    if (BufferID == 0 || BufferID > 20)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x08;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x04;
    
    // ��������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = BufferID;
    
    // ��ʼҳ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&StartPage)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&StartPage)[0];
    
    // ҳ��
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&PageNum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&PageNum)[0];
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x05 �������ļ��ϲ�����ģ����������ļ�
 */
int finger_mark_gen_cmd_RegModel(finger_mark_cmd_t * pfinger_mark_cmd)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x03;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x05;
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x06 �������������е��ļ����浽 flash ���ƿ���
 */
int finger_mark_gen_cmd_StoreChar(
     finger_mark_cmd_t * pfinger_mark_cmd, 
     uint8_t             BufferID, 
     uint16_t            PageID)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    if (BufferID == 0 || BufferID > 20)
    {
        return FINGER_MARK_ERROR;
    }
    if (PageID == 0 || PageID > 80)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x06;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x06;
    
    // ��������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = BufferID;
    
    // λ�ú�
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&PageID)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&PageID)[0];
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x0c ɾ�� flash ���ƿ��е�һ�������ļ�
 */
int finger_mark_gen_cmd_DeletChar(
     finger_mark_cmd_t * pfinger_mark_cmd,  
     uint16_t            PageID, 
     uint16_t            PageNum)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    if (PageNum == 0 || PageNum > 80)
    {
        return FINGER_MARK_ERROR;
    }
    if (PageID == 0 || PageID > 80)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x06;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x06;
    
    // ɾ����ʼλ�ú�
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&PageID)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&PageID)[0];
    
    // ɾ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&PageNum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&PageNum)[0];
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x0d ��� flash ָ�ƿ�
 */
int finger_mark_gen_cmd_Empty(finger_mark_cmd_t * pfinger_mark_cmd)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x03;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x0d;
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x0f ��ϵͳ��������
 */
int finger_mark_gen_cmd_ReadSysPara(finger_mark_cmd_t * pfinger_mark_cmd)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x03;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x0f;
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x14 ���������
 */
int finger_mark_gen_cmd_GetRandomCode(finger_mark_cmd_t * pfinger_mark_cmd)
{
    const uint8_t cmd[] = {
        0xef, 0x01, 0xff, 0xff, 0xff, 0xff, 
    0x01, 
    0x00, 0x03, 
    0x14, 
    0x00, 0x18};
    uint32_t length = sizeof(cmd) / sizeof(cmd[0]);
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    
    for (uint32_t i = 0; i < length; i++)
    {
        pfinger_mark_cmd->data[i] = cmd[i];
    //    while (app_uart_put(cmd[i]) != NRF_SUCCESS);
        
    }
    pfinger_mark_cmd->len = length;
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x1d ����Чģ�����
 */
int finger_mark_gen_cmd_ValidTempleteNum(finger_mark_cmd_t * pfinger_mark_cmd)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x03;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x1d;
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x1f ��������
 */
int finger_mark_gen_cmd_ReadIndexTable(finger_mark_cmd_t * pfinger_mark_cmd, uint8_t index_table_page_ID)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x04;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x1f;
    
    // ҳ��
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = index_table_page_ID;
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

/*
    0x29 ע���û�ȡͼ��
 */
int finger_mark_gen_cmd_GetEnrollImage(finger_mark_cmd_t * pfinger_mark_cmd)
{
    uint16_t sum = 0, cnt = 0;
    
    if (pfinger_mark_cmd == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    
    pfinger_mark_cmd->len = 0;
    
    // ��ͷ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xef;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ��ַ
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0xff;
    
    // ����־
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x01;
    
    // ������
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x00;
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x03;
    
    // ָ����
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = 0x29;
    
    sum = 0;
    for (cnt = 6; cnt < pfinger_mark_cmd->len; cnt++)
    {
        sum += pfinger_mark_cmd->data[cnt];
    }
    
    // У���
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[1];
    pfinger_mark_cmd->data[pfinger_mark_cmd->len++] = ((uint8_t*)&sum)[0];
    
    return FINGER_MARK_SUCCESS;
}

// 
int FingerMarkDataProcess_init(
    finger_mark_receive_state_machine_t * preceive_state_machine)
{
    if (preceive_state_machine == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    preceive_state_machine->pack.is_pack_valid          = false;
    preceive_state_machine->sub_state_receive_char_len  = 0;
    preceive_state_machine->receive_state               = STATE_WAIT_PACK_HEAD;
    
    return FINGER_MARK_SUCCESS;
}

/*
    ������ָ��ģ���յ������ݰ�
 */
int FingerMarkDataProcess(
    finger_mark_receive_state_machine_t * preceive_state_machine,
    uint8_t             c)
{
    uint16_t *psub_len  = NULL;
    uint8_t  *sub_data  = NULL;
    uint16_t *pdata_len = NULL;
    uint16_t *pcheck_sum_receive = NULL;
    
    if (preceive_state_machine == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    psub_len            = &(preceive_state_machine->sub_state_receive_char_len);
    sub_data            =   preceive_state_machine->sub_state_receive_data;
    pdata_len           = &(preceive_state_machine->data_state_receive_char_len);
    pcheck_sum_receive  = &(preceive_state_machine->check_sum_receive);
    
    switch (preceive_state_machine->receive_state)
    {
        case STATE_WAIT_PACK_HEAD:
            sub_data[(*psub_len)++] = c;
            if ((*psub_len) >= 2)
            {
                if (sub_data[0] == 0xef && sub_data[1] == 0x01)
                {
                    preceive_state_machine->receive_state = STATE_WAIT_ADDR;
                 //   send_string("Receive head", strlen("Receive head"));
                }
             //   send_string(sub_data, (*psub_len));
                (*psub_len) = 0;
            }
            break;
            
        case STATE_WAIT_ADDR:
            sub_data[(*psub_len)++] = c;
            if ((*psub_len) >= 4)
            {
                if (
                    sub_data[0] == 0xff &&
                    sub_data[1] == 0xff &&
                    sub_data[2] == 0xff &&
                    sub_data[3] == 0xff
                )
                {
                    preceive_state_machine->receive_state = STATE_WAIT_PACK_TYPE;
                    preceive_state_machine->check_sum_calc = 0;
                }
                else
                {
                    preceive_state_machine->receive_state = STATE_WAIT_PACK_HEAD;
                }
                (*psub_len) = 0;
                
                
            //    send_string("Receive addr", strlen("Receive addr"));
            }
            
            break;
            
        case STATE_WAIT_PACK_TYPE:
            sub_data[(*psub_len)++] = c;
            preceive_state_machine->check_sum_calc += c;
            if ((*psub_len) >= 1)
            {
                if (
                    sub_data[0] == PACK_DATA_WITH_NEXT  ||
                    sub_data[0] == PACK_DATA_ENDING     ||
                    sub_data[0] == PACK_CMD_REPLY
                )
                {
                    preceive_state_machine->receive_pack_type = (finger_mark_pack_type_t)sub_data[0];
                    preceive_state_machine->receive_state = STATE_WAIT_PACK_LEN;
                }
                else
                {
                    preceive_state_machine->receive_state = STATE_WAIT_PACK_HEAD;
                }
                (*psub_len) = 0;
                
            //    send_string("Rec pack type", strlen("Rec pack type"));
            }
            break;
            
        case STATE_WAIT_PACK_LEN:
            sub_data[(*psub_len)++] = c;
            preceive_state_machine->check_sum_calc += c;
            if ((*psub_len) >= 2)
            {
                *pdata_len = ((sub_data[0] << 8) | sub_data[1]) - 2;
                
                preceive_state_machine->receive_state = STATE_WAIT_PACK_DATA;
                
                (*psub_len) = 0;
                
            //    send_string("Rec pack len", strlen("Rec pack len"));
            }
            break;
            
        case STATE_WAIT_PACK_DATA:
            sub_data[(*psub_len)++] = c;
            preceive_state_machine->check_sum_calc += c;
            if ((*psub_len) >= (*pdata_len))
            {
                preceive_state_machine->receive_state = STATE_WAIT_CHECK_SUM;
                (*psub_len) = 0;
            }
            break;
            
        case STATE_WAIT_CHECK_SUM:
            if ((*psub_len) == 0)
            {
                ((uint8_t*)pcheck_sum_receive)[1] = c;
                (*psub_len)++;
            }
            else if ((*psub_len) == 1)
            {
                ((uint8_t*)pcheck_sum_receive)[0] = c;
                if (*pcheck_sum_receive == preceive_state_machine->check_sum_calc)
                {
                    // ������ݰ�
                    
                    if (preceive_state_machine->pack.is_pack_valid  == false)
                    {
                        preceive_state_machine->pack.pack_type      = preceive_state_machine->receive_pack_type;
                        preceive_state_machine->pack.pack_len       = *pdata_len;
                        preceive_state_machine->pack.is_pack_valid  = true;
                        for (uint16_t i = 0; i < *pdata_len; i++)
                        {
                            preceive_state_machine->pack.pack_data[i] = sub_data[i];
                        }
                    //    send_string("Receive Pack", strlen("Receive Pack"));
                        send_string_tx(sub_data, *pdata_len);
                    }
                    else
                    {
                        //
                     //   send_string("Pack overflow", strlen("Pack overflow"));
                    }
                }
                else
                {
                    // У��ʹ���
                    send_string("sum err", strlen("sum err"));
                }
                (*psub_len) = 0;
                preceive_state_machine->receive_state = STATE_WAIT_PACK_HEAD;
            }
            else
            {
                preceive_state_machine->receive_state = STATE_WAIT_PACK_HEAD;
            }
            break;
            
        default:
            break;
    }
    
    return FINGER_MARK_SUCCESS;
}


int finger_mark_manage_state_machine_init(
        finger_mark_manage_state_machine_t *pmanage_state)
{
    if (pmanage_state == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    pmanage_state->touch_state.is_finger_touch_pushed = false;
    pmanage_state->touch_state.is_finger_touch_pushed_last = true;
    pmanage_state->touch_state.is_finger_on = false;
    pmanage_state->touch_state.is_finger_off = false;   
    pmanage_state->touch_state.is_long_push  = false;
    pmanage_state->touch_state.long_push_cnt  = 0;
    
    
    pmanage_state->manage_state = MANAGE_STATE_POWER;
    pmanage_state->state_power = MANAGE_STATE_POWER_SLEEPING;
    pmanage_state->data_power.next_state = MANAGE_STATE_RECOGNITION_FINGER_MARK;
    pmanage_state->state_recognition = MANAGE_STATE_RECOGNITION_START;
    
    
    pmanage_state->data_read_index_table.next_state =   MANAGE_STATE_RECOGNITION_FINGER_MARK;
    
//    nrf_gpio_pin_write(FENGER_TOUCH_ON_PIN_NUMBER, FENGER_TOUCH_ON_ACTIVE_LEVEL ? 1 : 0);
    
    return FINGER_MARK_SUCCESS;
}


/*
    ��鴥��״̬
 */
int finger_mark_check_touch_state(
        finger_mark_manage_state_machine_t *pmanage_state)
{
    if (pmanage_state == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    
    // ���touch
    if (nrf_gpio_pin_read(FENGER_TOUCH_PIN_NUMBER) == 1)
    {
        pmanage_state->touch_state.is_finger_touch_pushed = true;
        
        if (pmanage_state->touch_state.long_push_cnt >= MS_TO_CNT(4000, TIMER1_CIRCLE_MS))
        {
            pmanage_state->touch_state.long_push_cnt = 0;
            pmanage_state->touch_state.is_long_push = true;
        }
        else
        {
            pmanage_state->touch_state.long_push_cnt++;
        }
    }
    else
    {
        pmanage_state->touch_state.is_finger_touch_pushed = false;
        pmanage_state->touch_state.long_push_cnt = 0;
    }
    
    if (pmanage_state->touch_state.is_finger_touch_pushed 
        != 
        pmanage_state->touch_state.is_finger_touch_pushed_last)
    {
        if (pmanage_state->touch_state.is_finger_touch_pushed)
        {
            pmanage_state->touch_state.is_finger_on    = true;
        }
        else
        {
            pmanage_state->touch_state.is_finger_off   = true;
        }
        
        if (pmanage_state->touch_state.is_finger_touch_pushed)
        {
            nrf_gpio_pin_write(LED_PIN_NUMBER, LEDS_ACTIVE_STATE ? 1 : 0);
        }
        else
        {
            nrf_gpio_pin_write(LED_PIN_NUMBER, LEDS_ACTIVE_STATE ? 0 : 1);
        }
        pmanage_state->touch_state.is_finger_touch_pushed_last 
        = 
        pmanage_state->touch_state.is_finger_touch_pushed;
    }
    
    return FINGER_MARK_SUCCESS;
}

/*
    ������״̬
 */
int finger_mark_check_manage_state(
        finger_mark_manage_state_machine_t *pmanage_state)
{
    if (pmanage_state == NULL)
    {
        return FINGER_MARK_ERROR;
    }
    
    switch (pmanage_state->user_cmd.user_cmd_type)
    {
        case USER_CMD_NONE:
            break;
        
        case USER_CMD_RECOGNITION_FINGER_MARK:
            if (pmanage_state->manage_state == MANAGE_STATE_IDLE ||
                pmanage_state->manage_state == MANAGE_STATE_RECOGNITION_FINGER_MARK)
            {
                pmanage_state->manage_state = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                
                pmanage_state->user_cmd.user_cmd_type = USER_CMD_NONE;
            }
            else if (pmanage_state->manage_state == MANAGE_STATE_POWER) // ˯����
            {
                if (pmanage_state->data_power.next_state == MANAGE_STATE_RECOGNITION_FINGER_MARK)
                {
                    pmanage_state->state_power = MANAGE_STATE_POWER_WAKE_UP_START;
                    
                    pmanage_state->user_cmd.user_cmd_type = USER_CMD_NONE;
                }
            }
            
            break;
        
        case USER_CMD_ENROLL_FINGER_MARK:
            if (pmanage_state->manage_state == MANAGE_STATE_IDLE ||
                (pmanage_state->manage_state == MANAGE_STATE_RECOGNITION_FINGER_MARK &&
                 pmanage_state->state_recognition == MANAGE_STATE_RECOGNITION_GetImage_Send)
            )
            {
                pmanage_state->manage_state = MANAGE_STATE_ENROLL_FINGER_MARK;
                pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_START;
                
                pmanage_state->user_cmd.user_cmd_type = USER_CMD_NONE;
            }
            else if (pmanage_state->manage_state == MANAGE_STATE_POWER) // ˯����
            {
                if (pmanage_state->data_power.next_state == MANAGE_STATE_RECOGNITION_FINGER_MARK)
                {
                    pmanage_state->data_power.next_state = MANAGE_STATE_ENROLL_FINGER_MARK;
                    pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_START;
                    
                    pmanage_state->state_power = MANAGE_STATE_POWER_WAKE_UP_START;
                    
                    pmanage_state->user_cmd.user_cmd_type = USER_CMD_NONE;
                }
            }
                        
            break;
            
        case USER_CMD_ENROLL_FINGER_MARK_CANCEL:
            if (pmanage_state->manage_state == MANAGE_STATE_ENROLL_FINGER_MARK /*&&
                 pmanage_state->state_enroll == MANAGE_STATE_RECOGNITION_GetImage_Send */
            )
            {
                pmanage_state->manage_state = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                pmanage_state->state_recognition    =   MANAGE_STATE_RECOGNITION_START;
                
                snprintf(string, STRING_LEN, "cancel_reg_cr");
                send_string((uint8_t*)string, strlen(string));
                
                pmanage_state->user_cmd.user_cmd_type = USER_CMD_NONE;
            }
            else 
            {
                pmanage_state->user_cmd.user_cmd_type = USER_CMD_NONE;
            }
            
            break;
            
        case USER_CMD_CLEAR_ALL_FINGER_MARK:
            if (pmanage_state->manage_state == MANAGE_STATE_IDLE ||
                (pmanage_state->manage_state == MANAGE_STATE_RECOGNITION_FINGER_MARK &&
                 pmanage_state->state_recognition == MANAGE_STATE_RECOGNITION_GetImage_Send)
            )
            {
                pmanage_state->manage_state = MANAGE_STATE_CLEAR_ALL_FINGER_MARK;
                pmanage_state->state_clear_all_finger = MANAGE_STATE_CLEAR_START;
                
                pmanage_state->user_cmd.user_cmd_type = USER_CMD_NONE;
            }
            else if (pmanage_state->manage_state == MANAGE_STATE_POWER) // ˯����
            {
                if (pmanage_state->data_power.next_state == MANAGE_STATE_RECOGNITION_FINGER_MARK)
                {
                    pmanage_state->data_power.next_state  = MANAGE_STATE_CLEAR_ALL_FINGER_MARK;
                    pmanage_state->state_clear_all_finger = MANAGE_STATE_CLEAR_START;
                    
                    pmanage_state->state_power = MANAGE_STATE_POWER_WAKE_UP_START;
                    
                    pmanage_state->user_cmd.user_cmd_type = USER_CMD_NONE;
                }
            }
                    
            break;
            
        default:
            break;
    }
    
    if (pmanage_state->touch_state.is_long_push == true)
    {
        if (pmanage_state->manage_state == MANAGE_STATE_ENROLL_FINGER_MARK /*&&
             pmanage_state->state_enroll == MANAGE_STATE_RECOGNITION_GetImage_Send */
        )
        {
            pmanage_state->manage_state = MANAGE_STATE_RECOGNITION_FINGER_MARK;
            pmanage_state->state_recognition    =   MANAGE_STATE_RECOGNITION_START;
            
            snprintf(string, STRING_LEN, "cancel_reg_lp");
            send_string((uint8_t*)string, strlen(string));
        }
    }
    pmanage_state->touch_state.is_long_push = false;
    
    static int enroll_led_flash_cnt = 0;
    if (pmanage_state->manage_state == MANAGE_STATE_ENROLL_FINGER_MARK) // ����
    {
        
        if (enroll_led_flash_cnt >= MS_TO_CNT(500, TIMER1_CIRCLE_MS))
        {
            // �Ʒ�ת
            nrf_gpio_pin_toggle(LED_PIN_NUMBER);
            enroll_led_flash_cnt = 0;
        }
        else
        {
            enroll_led_flash_cnt++;
        }
    }
    else
    {
        // ��
        if (pmanage_state->touch_state.is_finger_touch_pushed 
            == 
            pmanage_state->touch_state.is_finger_touch_pushed_last
        )
        {
            if (pmanage_state->touch_state.is_finger_touch_pushed)
            {
                nrf_gpio_pin_write(LED_PIN_NUMBER, LEDS_ACTIVE_STATE ? 1 : 0);
            }
            else
            {
                nrf_gpio_pin_write(LED_PIN_NUMBER, LEDS_ACTIVE_STATE ? 0 : 1);
            }
        }
        
        enroll_led_flash_cnt = 0;
    }
    
    switch (pmanage_state->manage_state)
    {
        case MANAGE_STATE_ENROLL_FINGER_MARK:
            
            switch (pmanage_state->state_enroll)
            {
                case MANAGE_STATE_ENROLL_FINGER_MARK_START:
                    pmanage_state->data_enroll.enroll_cnt           = 1;
                    pmanage_state->data_enroll.enroll_cnt_last      = 0;
                    pmanage_state->data_enroll.enroll_N             = 4;
                
                    pmanage_state->data_enroll.store_page_ID  = 0;
                    pmanage_state->data_enroll.is_read_index_table = false;
                
              //      pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Send;
                
                    pmanage_state->data_read_index_table.next_state = MANAGE_STATE_ENROLL_FINGER_MARK;
                    
                    pmanage_state->manage_state =   MANAGE_STATE_READ_INDEX_TABLE;
                    pmanage_state->state_read_index_table = MANAGE_STATE_READ_INDEX_TABLE_START;
                
                    pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_READ_INDEX_TABLE;
                    
                    break;
                
                case MANAGE_STATE_ENROLL_FINGER_MARK_READ_INDEX_TABLE:
                    
                    if (pmanage_state->data_read_index_table.is_table_read)
                    {
                        uint8_t *index_value = NULL;
                        uint16_t   byte_cnt = 0, byte_data, bit_cnt;
                        
                        pmanage_state->data_enroll.is_read_index_table = true;
                        
                        index_value     = pmanage_state->data_read_index_table.index_value[0];
                        
                        //templete_num = 0,
                        
                        for (byte_cnt = 0; byte_cnt < 32; byte_cnt ++)
                        {
                            byte_data = index_value[byte_cnt];
                            
                            for (bit_cnt = 0; bit_cnt < 8; bit_cnt ++)
                            {
                                if ((byte_data & (0x01 << bit_cnt)) == 0)
                                {
                               //     templete_num++;
                                    if (!(bit_cnt == 0 && byte_cnt == 0))
                                    {
                                        pmanage_state->data_enroll.store_page_ID = byte_cnt * 8 + bit_cnt;
                                        
                                        bit_cnt     =   8 + 1;
                                        byte_cnt    =   32 + 1;
                                    }
                                }
                            }
                        }
                            
                        pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Send;
                    }
                    else
                    {
                        pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_FAILURE;
                    }
                    
                    
                    break;
                
                case MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Send:
                
                    if (pmanage_state->data_enroll.enroll_cnt 
                        != 
                        pmanage_state->data_enroll.enroll_cnt_last
                    )
                    {
                        snprintf(string, STRING_LEN, "push finger:%1d", pmanage_state->data_enroll.enroll_cnt);
                        send_string((uint8_t*)string, strlen(string));
                        
                        pmanage_state->data_enroll.enroll_cnt_last 
                         =
                        pmanage_state->data_enroll.enroll_cnt;
                    }
                
                    if (pmanage_state->touch_state.is_finger_on && /**/
                        pmanage_state->touch_state.is_finger_touch_pushed
                    )
                    {
                        pmanage_state->touch_state.is_finger_on = false;
                    //    pmanage_state->data_enroll.enroll_cnt  ++;
                    
                        FingerMarkDataProcess_init(
                            &g_finger_mark_receive_state_machine);
                        finger_mark_gen_cmd_GetEnrollImage(&g_finger_mark_cmd);
                        finger_mark_send_cmd(&g_finger_mark_cmd);
                        
                        pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Wait_Reply;
                    }
                    break;
                
                case MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Wait_Reply:
                    if (g_finger_mark_receive_state_machine.pack.is_pack_valid)
                    {
                        if (g_finger_mark_receive_state_machine.pack.pack_type == PACK_CMD_REPLY &&
                            g_finger_mark_receive_state_machine.pack.pack_len == 1 && 
                            g_finger_mark_receive_state_machine.pack.pack_data[0] == 0
                        )
                        {
                            
                            snprintf(string, STRING_LEN, "finger success");
                            send_string((uint8_t*)string, strlen(string));
                        
                            // ȡ��ָ��ͼ�� ��������ȥ
                            pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_GenChar_Send;
                        }
                        else
                        {
                            
//                            snprintf(string, STRING_LEN, "finger fail");
//                            send_string((uint8_t*)string, strlen(string));
//                            
//                            pmanage_state->data_enroll.enroll_cnt_last--;
                        
                            // ����ȡָ��
                            pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Send;
                        }
                        
                        g_finger_mark_receive_state_machine.pack.is_pack_valid = false;
                    }
                
                    break;
                    
                case MANAGE_STATE_ENROLL_FINGER_MARK_GenChar_Send:
                    
                    FingerMarkDataProcess_init(
                        &g_finger_mark_receive_state_machine);
                    finger_mark_gen_cmd_GenChar(&g_finger_mark_cmd, 
                          pmanage_state->data_enroll.enroll_cnt);
                    finger_mark_send_cmd(&g_finger_mark_cmd);
                
                    pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_GenChar_Wait_Reply;
                
                    break;
                
                case MANAGE_STATE_ENROLL_FINGER_MARK_GenChar_Wait_Reply:
                    
                    if (g_finger_mark_receive_state_machine.pack.is_pack_valid)
                    {
                        if (g_finger_mark_receive_state_machine.pack.pack_type == PACK_CMD_REPLY &&
                            g_finger_mark_receive_state_machine.pack.pack_len == 1 && 
                            g_finger_mark_receive_state_machine.pack.pack_data[0] == 0
                        )
                        {
                            
//                            snprintf(string, STRING_LEN, "finger get success");
//                            send_string(string, strlen(string));
                        
                            // �жϴ����ﵽ �ϲ�����ȥ
                            if (pmanage_state->data_enroll.enroll_cnt 
                                >= 
                                pmanage_state->data_enroll.enroll_N)
                            {
                                pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_RegModel_Send;
                            }
                            else
                            {
                                pmanage_state->data_enroll.enroll_cnt ++;
                                
                                // ��ȡͼ��
                                pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Send;
                            }
                        }
                        else
                        {
                            // ��������ʧ��
                            snprintf(string, STRING_LEN, "gen char fail");
                            send_string((uint8_t*)string, strlen(string));
                            pmanage_state->data_enroll.enroll_cnt_last--;
                            // ����ȡָ��
                            pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_GetEnrollImage_Send;
                        }
                        
                        g_finger_mark_receive_state_machine.pack.is_pack_valid = false;
                    }
                
                    break;
                
                case MANAGE_STATE_ENROLL_FINGER_MARK_RegModel_Send:
                    
                    FingerMarkDataProcess_init(
                        &g_finger_mark_receive_state_machine);
                    finger_mark_gen_cmd_RegModel(&g_finger_mark_cmd);
                    finger_mark_send_cmd(&g_finger_mark_cmd);
                
                    pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_RegModel_Wait_Reply;
                
                    break;
                
                case MANAGE_STATE_ENROLL_FINGER_MARK_RegModel_Wait_Reply:
                    if (g_finger_mark_receive_state_machine.pack.is_pack_valid)
                    {
                        if (g_finger_mark_receive_state_machine.pack.pack_type == PACK_CMD_REPLY &&
                            g_finger_mark_receive_state_machine.pack.pack_len == 1 && 
                            g_finger_mark_receive_state_machine.pack.pack_data[0] == 0
                        )
                        {
                            
                            snprintf(string, STRING_LEN, "RegModel success");
                            send_string((uint8_t*)string, strlen(string));
                        
                            // ����ģ�� �洢ģ��ȥ
                            pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_StoreChar_Send;
                        }
                        else
                        {
                            
                            snprintf(string, STRING_LEN, "RegModel failure");
                            send_string((uint8_t*)string, strlen(string));
                        
                            // ע��ʧ��
                            pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_FAILURE;
                        }
                        
                        g_finger_mark_receive_state_machine.pack.is_pack_valid = false;
                    }
                    break;
                
                case MANAGE_STATE_ENROLL_FINGER_MARK_StoreChar_Send:
                    
                    FingerMarkDataProcess_init(
                         &g_finger_mark_receive_state_machine);
                    finger_mark_gen_cmd_StoreChar(
                         &g_finger_mark_cmd, 
                         1,     // BufferID
                         pmanage_state->data_enroll.store_page_ID);    // PageID
                    finger_mark_send_cmd(&g_finger_mark_cmd);
                
                    pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_StoreChar_Wait_Reply;
                
                    break;
                
                case MANAGE_STATE_ENROLL_FINGER_MARK_StoreChar_Wait_Reply:
                    if (g_finger_mark_receive_state_machine.pack.is_pack_valid)
                    {
                        if (g_finger_mark_receive_state_machine.pack.pack_type == PACK_CMD_REPLY &&
                            g_finger_mark_receive_state_machine.pack.pack_len == 1 && 
                            g_finger_mark_receive_state_machine.pack.pack_data[0] == 0
                        )
                        {
                            
                            snprintf(string, STRING_LEN, "StoreChar success");
                            send_string((uint8_t*)string, strlen(string));
                        
                            // �洢ģ�� ע��ɹ�
                            pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_SECCUSS;
                        }
                        else
                        {
                            snprintf(string, STRING_LEN, "StoreChar failure");
                            send_string((uint8_t*)string, strlen(string));
                        
                            // ע��ʧ��
                            pmanage_state->state_enroll = MANAGE_STATE_ENROLL_FINGER_MARK_FAILURE;
                        }
                        
                        g_finger_mark_receive_state_machine.pack.is_pack_valid = false;
                    }
                    break;
                
                case MANAGE_STATE_ENROLL_FINGER_MARK_SECCUSS:
                    
                    snprintf(string, STRING_LEN, "enroll ok ID:%d", pmanage_state->data_enroll.store_page_ID);
                    send_string((uint8_t*)string, strlen(string));
                        
                //    pmanage_state->manage_state = MANAGE_STATE_IDLE;
//                
//                    pmanage_state->manage_state         = MANAGE_STATE_RECOGNITION_FINGER_MARK;
//                    pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_START;
//                                
                    pmanage_state->manage_state             = MANAGE_STATE_READ_INDEX_TABLE;
                    pmanage_state->state_read_index_table   = MANAGE_STATE_READ_INDEX_TABLE_START;
                    
                    break;
                    
                case MANAGE_STATE_ENROLL_FINGER_MARK_FAILURE:
                    
                    snprintf(string, STRING_LEN, "enroll fail%d", pmanage_state->data_enroll.store_page_ID);
                    send_string((uint8_t*)string, strlen(string));
                        
               //     pmanage_state->manage_state = MANAGE_STATE_IDLE;
                
                    pmanage_state->manage_state         = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                    pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_START;
                    
                    break;
                    
                default:
                    break;
            }
        
            break;
        
        case MANAGE_STATE_RECOGNITION_FINGER_MARK:
            
            switch (pmanage_state->state_recognition)
            {
                case MANAGE_STATE_RECOGNITION_START:                     // ��ʼʶ��
                    pmanage_state->data_recognition.BufferID        = 1;
                    pmanage_state->data_recognition.ret_page_ID     = 0xffff;
                    pmanage_state->data_recognition.ret_match_score = 0;
                    pmanage_state->data_recognition.start_page      = 0;
                    pmanage_state->data_recognition.page_num        = 60 - 1;
                
                    pmanage_state->data_power.sleep_timeout_cnt     =   0;
                    pmanage_state->data_power.sleep_timeout_N       =   MS_TO_CNT(10 * 1000, TIMER1_CIRCLE_MS);      // 10 * 1000 ms (10 s) �� ָ��ģ��˯��          
                
                    pmanage_state->state_recognition    =   MANAGE_STATE_RECOGNITION_GetImage_Send;
                    break;
    
                case MANAGE_STATE_RECOGNITION_GetImage_Send:             // ���ͻ�ȡͼ��ָ��
                    
                
                    if (pmanage_state->touch_state.is_finger_on &&  /*  */
                        pmanage_state->touch_state.is_finger_touch_pushed
                    )
                    {
                        pmanage_state->touch_state.is_finger_on = false;
                    
                        
                        FingerMarkDataProcess_init(
                            &g_finger_mark_receive_state_machine);
                        finger_mark_gen_cmd_GetImage(&g_finger_mark_cmd);
                        finger_mark_send_cmd(&g_finger_mark_cmd);
                        
                        pmanage_state->state_recognition = MANAGE_STATE_RECOGNITION_GetImage_Wait_Reply;
                        
                        /*
                        if (g_open_door_manage.cmd_state == OPEN_DOOR_CMD_NAN)
                        {
                            g_open_door_manage.cmd_state = OPEN_DOOR_CMD_OPEN;
                        }
                            
                        open_cmd_start();
                        */
                    }
                    else
                    {
                        // ��ʱ
                        if (pmanage_state->data_power.sleep_timeout_cnt >= 
                            pmanage_state->data_power.sleep_timeout_N)
                        {
                            pmanage_state->data_power.sleep_timeout_cnt = 0;
                            
                            pmanage_state->data_power.next_state   =   MANAGE_STATE_RECOGNITION_FINGER_MARK;
                            pmanage_state->manage_state =   MANAGE_STATE_POWER;
                            pmanage_state->state_power  =   MANAGE_STATE_POWER_SLEEP_START;
                        }
                        else
                        {
                            pmanage_state->data_power.sleep_timeout_cnt++;
                        }
                    }
                    
                    break;
                
                case MANAGE_STATE_RECOGNITION_GetImage_Wait_Reply:       // Ӧ��
                    if (g_finger_mark_receive_state_machine.pack.is_pack_valid)
                    {
                        if (g_finger_mark_receive_state_machine.pack.pack_type == PACK_CMD_REPLY &&
                            g_finger_mark_receive_state_machine.pack.pack_len == 1 && 
                            g_finger_mark_receive_state_machine.pack.pack_data[0] == 0
                        )
                        {
                            
                            snprintf(string, STRING_LEN, "finger success");
                            send_string((uint8_t*)string, strlen(string));
                        
                            // ȡ��ָ��ͼ�� ��������ȥ
                            pmanage_state->state_recognition = MANAGE_STATE_RECOGNITION_GenChar_Send;
                        }
                        else
                        {
                            
                            snprintf(string, STRING_LEN, "finger fail");
                            send_string((uint8_t*)string, strlen(string));
                        
                            // ����ȡָ��
                            pmanage_state->state_recognition = MANAGE_STATE_RECOGNITION_GetImage_Send;
                        }
                        
                        g_finger_mark_receive_state_machine.pack.is_pack_valid = false;
                    }
                
                    break;
                
                case MANAGE_STATE_RECOGNITION_GenChar_Send:              // ������������ָ��
                    
                    FingerMarkDataProcess_init(
                        &g_finger_mark_receive_state_machine);
                    finger_mark_gen_cmd_GenChar(&g_finger_mark_cmd, 
                          pmanage_state->data_recognition.BufferID);
                    finger_mark_send_cmd(&g_finger_mark_cmd);
                
                    pmanage_state->state_recognition = MANAGE_STATE_RECOGNITION_GenChar_Wait_Reply;
                
                    break;
                
                case MANAGE_STATE_RECOGNITION_GenChar_Wait_Reply:        // Ӧ��
                    
                    if (g_finger_mark_receive_state_machine.pack.is_pack_valid)
                    {
                        if (g_finger_mark_receive_state_machine.pack.pack_type == PACK_CMD_REPLY &&
                            g_finger_mark_receive_state_machine.pack.pack_len == 1 && 
                            g_finger_mark_receive_state_machine.pack.pack_data[0] == 0
                        )
                        {
                            
//                            snprintf(string, STRING_LEN, "gen char success");
//                            send_string(string, strlen(string));

                            pmanage_state->state_recognition = MANAGE_STATE_RECOGNITION_Search_Send;

                        }
                        else
                        {
                            // ��������ʧ��
                            snprintf(string, STRING_LEN, "gen char fail");
                            send_string((uint8_t*)string, strlen(string));
                            
                            // ����ȡָ��
                            pmanage_state->state_recognition = MANAGE_STATE_RECOGNITION_GetImage_Send;
                        }
                        
                        g_finger_mark_receive_state_machine.pack.is_pack_valid = false;
                    }
                
                    break;
    
                case MANAGE_STATE_RECOGNITION_Search_Send:               // ����ָ��ָ��
                    
                    FingerMarkDataProcess_init(
                        &g_finger_mark_receive_state_machine);
                    
                    finger_mark_gen_cmd_Search(
                        &g_finger_mark_cmd, 
                        pmanage_state->data_recognition.BufferID , //  BufferID
                        pmanage_state->data_recognition.start_page,
                        pmanage_state->data_recognition.page_num);
                    finger_mark_send_cmd(&g_finger_mark_cmd);
                
                    pmanage_state->state_recognition = MANAGE_STATE_RECOGNITION_Search_Wait_Reply;
                
                    break;
                
                case MANAGE_STATE_RECOGNITION_Search_Wait_Reply:         // Ӧ��
                    
                    if (g_finger_mark_receive_state_machine.pack.is_pack_valid)
                    {
                        if (g_finger_mark_receive_state_machine.pack.pack_type == PACK_CMD_REPLY &&
                            g_finger_mark_receive_state_machine.pack.pack_len == 5 && 
                            g_finger_mark_receive_state_machine.pack.pack_data[0] == 0
                        )
                        {
                            pmanage_state->data_recognition.ret_page_ID  = 
                            (g_finger_mark_receive_state_machine.pack.pack_data[1] << 8) |
                            g_finger_mark_receive_state_machine.pack.pack_data[2];
                            
                            pmanage_state->data_recognition.ret_match_score  = 
                            (g_finger_mark_receive_state_machine.pack.pack_data[3] << 8) |
                            g_finger_mark_receive_state_machine.pack.pack_data[4];
                            
                            pmanage_state->state_recognition = MANAGE_STATE_RECOGNITION_Search_SUCCESS;

                        }
                        else
                        {
                            pmanage_state->state_recognition = MANAGE_STATE_RECOGNITION_Search_FAILURE;
                        }
                        
                        g_finger_mark_receive_state_machine.pack.is_pack_valid = false;
                    }
                
                    break;
    
                case MANAGE_STATE_RECOGNITION_Search_SUCCESS:            // �����ɹ�
                    
                    snprintf(string, STRING_LEN, "fg ID:%2d SC %2d", 
                        pmanage_state->data_recognition.ret_page_ID,
                        pmanage_state->data_recognition.ret_match_score);
                    send_string((uint8_t*)string, strlen(string));
                
                    if (g_open_door_manage.cmd_state == OPEN_DOOR_CMD_NAN)
                    {
                        g_open_door_manage.cmd_state = OPEN_DOOR_CMD_OPEN;
                    }
                        
                    open_cmd_start();
                
                    pmanage_state->manage_state         = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                 //   pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_GetImage_Send;
                    pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_START;

                    break;
                
                case MANAGE_STATE_RECOGNITION_Search_FAILURE:            // ����ʧ��
                    
                    snprintf(string, STRING_LEN, "search fail");
                    send_string((uint8_t*)string, strlen(string));
                
                    pmanage_state->manage_state         = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                 //   pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_GetImage_Send;
                    pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_START;
                
                    break;
                
                default:
                    break;
            }
        
            break;
        
        case MANAGE_STATE_IDLE:
            break;
        
        case MANAGE_STATE_CLEAR_ALL_FINGER_MARK:
            
            switch (pmanage_state->state_clear_all_finger)
            {
                case MANAGE_STATE_CLEAR_START:                  // ��ʼ���
                    pmanage_state->data_clear_all_finger.is_empty_accomplish = false;
                
                    pmanage_state->state_clear_all_finger = MANAGE_STATE_CLEAR_Empty_Send;
                    break;
                                
                case MANAGE_STATE_CLEAR_Empty_Send:             // �������ָ��ָ��
                    
                    FingerMarkDataProcess_init(
                        &g_finger_mark_receive_state_machine);
                    
                    finger_mark_gen_cmd_Empty(&g_finger_mark_cmd);
                    finger_mark_send_cmd(&g_finger_mark_cmd);
                
                    pmanage_state->state_clear_all_finger = MANAGE_STATE_CLEAR_Empty_Wait_Reply;
                    break;
                
                case MANAGE_STATE_CLEAR_Empty_Wait_Reply:       // Ӧ��
                    
                    if (g_finger_mark_receive_state_machine.pack.is_pack_valid)
                    {
                        if (g_finger_mark_receive_state_machine.pack.pack_type == PACK_CMD_REPLY &&
                            g_finger_mark_receive_state_machine.pack.pack_len == 1 && 
                            g_finger_mark_receive_state_machine.pack.pack_data[0] == 0
                        )
                        {
                            pmanage_state->data_clear_all_finger.is_empty_accomplish = true;
                            
                            pmanage_state->state_clear_all_finger = MANAGE_STATE_CLEAR_SUCCESS;
                        }
                        else
                        {
                            pmanage_state->state_clear_all_finger = MANAGE_STATE_CLEAR_FAILURE;
                        }
                        
                        g_finger_mark_receive_state_machine.pack.is_pack_valid = false;
                    }
                
                    break;
                    
                case MANAGE_STATE_CLEAR_SUCCESS:                // ���ָ�Ƴɹ�
                    
                    snprintf(string, STRING_LEN, "clear success");
                    send_string((uint8_t*)string, strlen(string));
                
                    pmanage_state->manage_state         = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                    pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_START;
                
                    break;
                
                case MANAGE_STATE_CLEAR_FAILURE:                // ���ָ��ʧ��
                    
                    snprintf(string, STRING_LEN, "clear failed");
                    send_string((uint8_t*)string, strlen(string));
                
                    pmanage_state->manage_state         = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                    pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_START;
                
                    break;
                    
                default:
                    break;
            }
            
            break;
            
        case MANAGE_STATE_READ_INDEX_TABLE:
            
            switch (pmanage_state->state_read_index_table)
            {
                case MANAGE_STATE_READ_INDEX_TABLE_START:
                    
                    pmanage_state->data_read_index_table.is_table_read          = false;
                    pmanage_state->data_read_index_table.read_timeout_cnt       = 0;
                    pmanage_state->data_read_index_table.read_timeout_N         = MS_TO_CNT(20, TIMER1_CIRCLE_MS);
                    pmanage_state->data_read_index_table.read_index_table_ID    =   0;
                
                    pmanage_state->state_read_index_table = MANAGE_STATE_READ_INDEX_TABLE_Send;
                
                    break;
                
                case MANAGE_STATE_READ_INDEX_TABLE_Send:
                    
                    FingerMarkDataProcess_init(
                        &g_finger_mark_receive_state_machine);
                    
                    finger_mark_gen_cmd_ReadIndexTable(
                        &g_finger_mark_cmd, 
                        pmanage_state->data_read_index_table.read_index_table_ID);
                    finger_mark_send_cmd(&g_finger_mark_cmd);
                
                    pmanage_state->data_read_index_table.read_timeout_cnt       = 0;
                    pmanage_state->state_read_index_table = MANAGE_STATE_READ_INDEX_TABLE_Wait_Reply;
                    
                    break;
                
                case MANAGE_STATE_READ_INDEX_TABLE_Wait_Reply:
                    
                    if (g_finger_mark_receive_state_machine.pack.is_pack_valid)
                    {
                        if (g_finger_mark_receive_state_machine.pack.pack_type == PACK_CMD_REPLY &&
                            g_finger_mark_receive_state_machine.pack.pack_len == 0x21 && 
                            g_finger_mark_receive_state_machine.pack.pack_data[0] == 0
                        )
                        {
                            uint8_t table_ID    = pmanage_state->data_read_index_table.read_index_table_ID;
                            uint8_t *rec_index_data = NULL;
                            uint8_t *index_value = NULL;
                            uint16_t  templete_num = 0, byte_cnt = 0, byte_data, bit_cnt;
                            
                            if (table_ID > 3)
                            {
                                table_ID = 0;
                            }
                            index_value     = pmanage_state->data_read_index_table.index_value[table_ID];
                            rec_index_data  = g_finger_mark_receive_state_machine.pack.pack_data + 1;
                            
                            memcpy(index_value, rec_index_data, 32);
                            
                            for (byte_cnt = 0; byte_cnt < 32; byte_cnt ++)
                            {
                                byte_data = rec_index_data[byte_cnt];
                                
                                for (bit_cnt = 0; bit_cnt < 8; bit_cnt ++)
                                {
                                    if (byte_data & (0x01 << bit_cnt))
                                    {
                                        templete_num++;
                                    }
                                }
                            }
                            
                        //    send_string(rec_index_data, 16);
                            send_string_tx(rec_index_data + 16, 16);
                            
                            pmanage_state->data_read_index_table.valid_templete_num[table_ID] = templete_num;
                            
                            pmanage_state->data_read_index_table.is_table_read = true;
                            
                            pmanage_state->state_read_index_table = MANAGE_STATE_READ_INDEX_TABLE_SUCCESS;
                        }
                        else
                        {
                            pmanage_state->state_read_index_table = MANAGE_STATE_READ_INDEX_TABLE_FAILURE;
                        }
                        
                        g_finger_mark_receive_state_machine.pack.is_pack_valid = false;
                    }
                    else if (pmanage_state->data_read_index_table.read_timeout_cnt
                           < pmanage_state->data_read_index_table.read_timeout_N)
                    {
                        pmanage_state->data_read_index_table.read_timeout_cnt++;
                    }
                    else {
                        pmanage_state->state_read_index_table = MANAGE_STATE_READ_INDEX_TABLE_FAILURE;
                    }
                
                    
                    break;
                
                case MANAGE_STATE_READ_INDEX_TABLE_SUCCESS:
                    
                    if (pmanage_state->data_read_index_table.next_state 
                        != 
                        MANAGE_STATE_RECOGNITION_FINGER_MARK
                    )
                    {
                        pmanage_state->manage_state         = pmanage_state->data_read_index_table.next_state;
                        pmanage_state->data_read_index_table.next_state = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                    }
                    else
                    {
                        pmanage_state->manage_state         = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                        pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_START;
                    }
                
                    break;
                    
                case MANAGE_STATE_READ_INDEX_TABLE_FAILURE:
                    
//                    pmanage_state->manage_state         = MANAGE_STATE_RECOGNITION_FINGER_MARK;
//                    pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_START;
                
                    if (pmanage_state->data_read_index_table.next_state 
                        != 
                        MANAGE_STATE_RECOGNITION_FINGER_MARK
                    )
                    {
                        pmanage_state->manage_state         = pmanage_state->data_read_index_table.next_state;
                        pmanage_state->data_read_index_table.next_state = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                    }
                    else
                    {
                        pmanage_state->manage_state         = MANAGE_STATE_RECOGNITION_FINGER_MARK;
                        pmanage_state->state_recognition    = MANAGE_STATE_RECOGNITION_START;
                    }
                
                    break;
                
                default:
                    break;
            }
        
            break;
            
        case MANAGE_STATE_POWER:
        
            switch (pmanage_state->state_power)
            {
                case MANAGE_STATE_POWER_WAKE_UP_START:
                    pmanage_state->data_power.wait_time_cnt =   0;
                    pmanage_state->data_power.wait_time_N   =   MS_TO_CNT(60, TIMER1_CIRCLE_MS);
                    
                    nrf_gpio_pin_write(FENGER_TOUCH_ON_PIN_NUMBER, FENGER_TOUCH_ON_ACTIVE_LEVEL ? 1 : 0);
                
                    // ������
                    uart_init();
                
                    pmanage_state->state_power = MANAGE_STATE_POWER_WAKE_UP_WAIT;
                
                    break;
                
                case MANAGE_STATE_POWER_WAKE_UP_WAIT:
                    
                    if (pmanage_state->data_power.wait_time_cnt 
                        <
                        pmanage_state->data_power.wait_time_N
                    )
                    {
                        pmanage_state->data_power.wait_time_cnt++;
                    }
                    else
                    {
                        pmanage_state->data_power.wait_time_cnt = 0;
                        
                        pmanage_state->data_power.sleep_timeout_cnt = 0;
                        
                        pmanage_state->manage_state = pmanage_state->data_power.next_state;
                        
                        snprintf(string, STRING_LEN, "finger wakeup");
                        send_string_tx((uint8_t*)string, strlen(string));
                    }
                    
                    break;
                
                case MANAGE_STATE_POWER_SLEEP_START:
                    
                    nrf_gpio_pin_write(FENGER_TOUCH_ON_PIN_NUMBER, FENGER_TOUCH_ON_ACTIVE_LEVEL ? 0 : 1);
                
                    pmanage_state->state_power = MANAGE_STATE_POWER_SLEEPING;
                
                    // �ش���
                    app_uart_close();
                    
                    snprintf(string, STRING_LEN, "finger sleeping");
                    send_string_tx((uint8_t*)string, strlen(string));
                
                    break;
                    
                case MANAGE_STATE_POWER_SLEEPING:
                    
                    
                    if (pmanage_state->touch_state.is_finger_on && /**/
                        pmanage_state->touch_state.is_finger_touch_pushed
                    )
                    {
                    //    pmanage_state->touch_state.is_finger_on = false;
                    
                        pmanage_state->state_power = MANAGE_STATE_POWER_WAKE_UP_START;
                    }
                                    
                    break;
                    
                default:
                    break;                
            }
        
            break;
            
        default:
            break;
    }
    
    return FINGER_MARK_SUCCESS;
}


