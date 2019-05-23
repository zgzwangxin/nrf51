/**
 * Copyright (c) 2012 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/* Attention!
*  To maintain compliance with Nordic Semiconductor ASA’s Bluetooth profile
*  qualification listings, this section of source code must not be modified.
*/
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_DIS)
#include "ble_dis_temp.h"

#include <stdlib.h>
#include <string.h>
#include "app_error.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"
#include "ble_l2cap.h"


int send_string(uint8_t * p_string, uint16_t length);
int send_string_tx(uint8_t * p_string, uint16_t length);
#define STRING_LEN  18
extern char string[STRING_LEN];


#define BLE_DIS_SYS_ID_LEN 8  /**< Length of System ID Characteristic Value. */
#define BLE_DIS_PNP_ID_LEN 7  /**< Length of Pnp ID Characteristic Value. */

//static uint16_t                 service_handle;
//static ble_gatts_char_handles_t manufact_name_handles;
//static ble_gatts_char_handles_t model_num_handles;
//static ble_gatts_char_handles_t serial_num_handles;
//static ble_gatts_char_handles_t hw_rev_handles;
//static ble_gatts_char_handles_t fw_rev_handles;
//static ble_gatts_char_handles_t sw_rev_handles;
//static ble_gatts_char_handles_t sys_id_handles;
//static ble_gatts_char_handles_t reg_cert_data_list_handles;
//static ble_gatts_char_handles_t pnp_id_handles;

#define OPCODE_LENGTH 1                                                    /**< Length of opcode inside Health Thermometer Measurement packet. */
#define HANDLE_LENGTH 2                                                    /**< Length of handle inside Health Thermometer Measurement packet. */
#define MAX_DIM_LEN   (BLE_L2CAP_MTU_DEF - OPCODE_LENGTH - HANDLE_LENGTH)  /**< Maximum size of a transmitted Health Thermometer Measurement. */


// Health Thermometer Measurement flag bits
#define DIS_MEAS_FLAG_TEMP_UNITS_BIT (0x01 << 0)  /**< Temperature Units flag. */
#define DIS_MEAS_FLAG_TIME_STAMP_BIT (0x01 << 1)  /**< Time Stamp flag. */
#define DIS_MEAS_FLAG_TEMP_TYPE_BIT  (0x01 << 2)  /**< Temperature Type flag. */



/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_hts       Health Thermometer Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_dis_t * p_dis, ble_evt_t * p_ble_evt)
{
    p_dis->conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
    p_dis->is_meas_notification_enabled = false;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_hts       Health Thermometer Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_dis_t * p_dis, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_dis->conn_handle = BLE_CONN_HANDLE_INVALID;
    p_dis->is_meas_notification_enabled = false;
}


/**@brief Function for handling write events to the Blood Pressure Measurement characteristic.
 *
 * @param[in]   p_hts         Health Thermometer Service structure.
 * @param[in]   p_evt_write   Write event received from the BLE stack.
 */
static void on_cccd_write(ble_dis_t * p_dis, ble_gatts_evt_write_t * p_evt_write)
{        
    if (p_evt_write->len == 2)
    {
        // CCCD written, update indication state

        ble_dis_evt_t evt;

//        if (ble_srv_is_indication_enabled(p_evt_write->data))
        if (ble_srv_is_notification_enabled(p_evt_write->data))
        {
            evt.evt_type = BLE_DIS_EVT_INDICATION_ENABLED;
            p_dis->is_meas_notification_enabled = true;
            
        }
        else
        {
            evt.evt_type = BLE_DIS_EVT_INDICATION_DISABLED;
            p_dis->is_meas_notification_enabled = false;
        }

        if (p_dis->evt_handler != NULL)
        {
            p_dis->evt_handler(p_dis, &evt);
        }
    }
}


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_hts       Health Thermometer Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_dis_t * p_dis, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_evt_write->handle == p_dis->meas_handles.cccd_handle)
    {
        on_cccd_write(p_dis, p_evt_write);
    }
}


/**@brief Function for handling the HVC event.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in]   p_hts       Health Thermometer Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_hvc(ble_dis_t * p_dis, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_hvc_t * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;

    if (p_hvc->handle == p_dis->meas_handles.value_handle)
    {
        ble_dis_evt_t evt;

        evt.evt_type = BLE_DIS_EVT_INDICATION_CONFIRMED;
        p_dis->evt_handler(p_dis, &evt);
    }
}


void ble_dis_on_ble_evt(ble_dis_t * p_dis, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_dis, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_dis, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_dis, p_ble_evt);
            break;

        case BLE_GATTS_EVT_HVC:
            on_hvc(p_dis, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for encoding a System ID.
 *
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 * @param[in]   p_sys_id           System ID to be encoded.
 */
static void sys_id_encode(uint8_t * p_encoded_buffer, const ble_dis_sys_id_t * p_sys_id)
{
    APP_ERROR_CHECK_BOOL(p_sys_id != NULL);
    APP_ERROR_CHECK_BOOL(p_encoded_buffer != NULL);

    p_encoded_buffer[0] = (p_sys_id->manufacturer_id & 0x00000000FF);
    p_encoded_buffer[1] = (p_sys_id->manufacturer_id & 0x000000FF00) >> 8;
    p_encoded_buffer[2] = (p_sys_id->manufacturer_id & 0x0000FF0000) >> 16;
    p_encoded_buffer[3] = (p_sys_id->manufacturer_id & 0x00FF000000) >> 24;
    p_encoded_buffer[4] = (p_sys_id->manufacturer_id & 0xFF00000000) >> 32;

    p_encoded_buffer[5] = (p_sys_id->organizationally_unique_id & 0x0000FF);
    p_encoded_buffer[6] = (p_sys_id->organizationally_unique_id & 0x00FF00) >> 8;
    p_encoded_buffer[7] = (p_sys_id->organizationally_unique_id & 0xFF0000) >> 16;
}


/**@brief Function for encoding a PnP ID.
 *
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 * @param[in]   p_pnp_id           PnP ID to be encoded.
 */
static void pnp_id_encode(uint8_t * p_encoded_buffer, const ble_dis_pnp_id_t * p_pnp_id)
{
    uint8_t len = 0;

    APP_ERROR_CHECK_BOOL(p_pnp_id != NULL);
    APP_ERROR_CHECK_BOOL(p_encoded_buffer != NULL);

    p_encoded_buffer[len++] = p_pnp_id->vendor_id_source;

    len += uint16_encode(p_pnp_id->vendor_id, &p_encoded_buffer[len]);
    len += uint16_encode(p_pnp_id->product_id, &p_encoded_buffer[len]);
    len += uint16_encode(p_pnp_id->product_version, &p_encoded_buffer[len]);

    APP_ERROR_CHECK_BOOL(len == BLE_DIS_PNP_ID_LEN);
}


/**@brief Function for adding the Characteristic.
 *
 * @param[in]   uuid           UUID of characteristic to be added.
 * @param[in]   p_char_value   Initial value of characteristic to be added.
 * @param[in]   char_len       Length of initial value. This will also be the maximum value.
 * @param[in]   dis_attr_md    Security settings of characteristic to be added.
 * @param[out]  p_handles      Handles of new characteristic.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t char_add(ble_dis_t                     * p_dis, 
                         uint16_t                        uuid,
                         uint8_t                       * p_char_value,
                         uint16_t                        char_len,
                         const ble_srv_security_mode_t * dis_attr_md,
                         ble_gatts_char_handles_t      * p_handles)
{
    ble_uuid_t          ble_uuid;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_md_t attr_md;

    APP_ERROR_CHECK_BOOL(p_char_value != NULL);
    APP_ERROR_CHECK_BOOL(char_len > 0);

    // The ble_gatts_char_md_t structure uses bit fields. So we reset the memory to zero.
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read  = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, uuid);

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = dis_attr_md->read_perm;
    attr_md.write_perm = dis_attr_md->write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = char_len;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = char_len;
    attr_char_value.p_value   = p_char_value;

    return sd_ble_gatts_characteristic_add(p_dis->service_handle, &char_md, &attr_char_value, p_handles);
}

/**@brief Function for encoding a Health Thermometer Measurement.
 *
 * @param[in]   p_hts              Health Thermometer Service structure.
 * @param[in]   p_hts_meas         Measurement to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t dis_measurement_encode(ble_dis_t      * p_dis,
                                      ble_dis_meas_t * p_dis_meas,
                                      uint8_t        * p_encoded_buffer)
{
    uint8_t  flags = 0;
    uint8_t  len   = 1;
    uint32_t encoded_temp;

    // Flags field
    if (p_dis_meas->temp_in_fahr_units)
    {
        flags |= DIS_MEAS_FLAG_TEMP_UNITS_BIT;
    }
    if (p_dis_meas->time_stamp_present)
    {
        flags |= DIS_MEAS_FLAG_TIME_STAMP_BIT;
    }

    // Temperature Measurement Value field
    if (p_dis_meas->temp_in_fahr_units)
    {
        flags |= DIS_MEAS_FLAG_TEMP_UNITS_BIT;

        encoded_temp = ((p_dis_meas->temp_in_fahr.exponent << 24) & 0xFF000000) |
                       ((p_dis_meas->temp_in_fahr.mantissa <<  0) & 0x00FFFFFF);
    }
    else
    {
        encoded_temp = ((p_dis_meas->temp_in_celcius.exponent << 24) & 0xFF000000) |
                       ((p_dis_meas->temp_in_celcius.mantissa <<  0) & 0x00FFFFFF);
    }
    len += uint32_encode(encoded_temp, &p_encoded_buffer[len]);

    // Time Stamp field
    if (p_dis_meas->time_stamp_present)
    {
        flags |= DIS_MEAS_FLAG_TIME_STAMP_BIT;
        len   += ble_date_time_encode(&p_dis_meas->time_stamp, &p_encoded_buffer[len]);
    }

    // Temperature Type field
    if (p_dis_meas->temp_type_present)
    {
        flags                  |= DIS_MEAS_FLAG_TEMP_TYPE_BIT;
        p_encoded_buffer[len++] = p_dis_meas->temp_type;
    }

    // Flags field
    p_encoded_buffer[0] = flags;

    return len;
}

static uint32_t dis_measurement_char_add(ble_dis_t * p_dis, const ble_dis_init_t * p_dis_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    ble_dis_meas_t      initial_dim;
    uint8_t             encoded_dim[MAX_DIM_LEN];

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    cccd_md.write_perm = p_dis_init->dis_meas_attr_md.cccd_write_perm;

    memset(&char_md, 0, sizeof(char_md));

    
    char_md.char_props.read     = 1;
//    char_md.char_props.indicate = 1;
    char_md.char_props.notify  = 1;
    char_md.p_char_user_desc    = NULL;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = &cccd_md;
    char_md.p_sccd_md           = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_TEMPERATURE_MEASUREMENT_CHAR);

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.read_perm  = p_dis_init->dis_meas_attr_md.read_perm;
    attr_md.write_perm = p_dis_init->dis_meas_attr_md.write_perm;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));
    memset(&initial_dim, 0, sizeof(initial_dim));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = dis_measurement_encode(p_dis, &initial_dim, encoded_dim);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_DIM_LEN;
    attr_char_value.p_value   = encoded_dim;

    return sd_ble_gatts_characteristic_add(p_dis->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_dis->meas_handles);
}


uint32_t ble_dis_init(ble_dis_t * p_dis, const ble_dis_init_t * p_dis_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Add service
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_DEVICE_INFORMATION_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_dis->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add characteristics
    if (p_dis_init->manufact_name_str.length > 0)
    {
        err_code = char_add(p_dis, 
                            BLE_UUID_MANUFACTURER_NAME_STRING_CHAR,
                            p_dis_init->manufact_name_str.p_str,
                            p_dis_init->manufact_name_str.length,
                            &p_dis_init->dis_attr_md,
                            &p_dis->manufact_name_handles);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    if (p_dis_init->model_num_str.length > 0)
    {
        err_code = char_add(p_dis, 
                            BLE_UUID_MODEL_NUMBER_STRING_CHAR,
                            p_dis_init->model_num_str.p_str,
                            p_dis_init->model_num_str.length,
                            &p_dis_init->dis_attr_md,
                            &p_dis->model_num_handles);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    if (p_dis_init->serial_num_str.length > 0)
    {
        err_code = char_add(p_dis, 
                            BLE_UUID_SERIAL_NUMBER_STRING_CHAR,
                            p_dis_init->serial_num_str.p_str,
                            p_dis_init->serial_num_str.length,
                            &p_dis_init->dis_attr_md,
                            &p_dis->serial_num_handles);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    if (p_dis_init->hw_rev_str.length > 0)
    {
        err_code = char_add(p_dis, 
                            BLE_UUID_HARDWARE_REVISION_STRING_CHAR,
                            p_dis_init->hw_rev_str.p_str,
                            p_dis_init->hw_rev_str.length,
                            &p_dis_init->dis_attr_md,
                            &p_dis->hw_rev_handles);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    if (p_dis_init->fw_rev_str.length > 0)
    {
        err_code = char_add(p_dis, 
                            BLE_UUID_FIRMWARE_REVISION_STRING_CHAR,
                            p_dis_init->fw_rev_str.p_str,
                            p_dis_init->fw_rev_str.length,
                            &p_dis_init->dis_attr_md,
                            &p_dis->fw_rev_handles);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    if (p_dis_init->sw_rev_str.length > 0)
    {
        err_code = char_add(p_dis, 
                            BLE_UUID_SOFTWARE_REVISION_STRING_CHAR,
                            p_dis_init->sw_rev_str.p_str,
                            p_dis_init->sw_rev_str.length,
                            &p_dis_init->dis_attr_md,
                            &p_dis->sw_rev_handles);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    if (p_dis_init->p_sys_id != NULL)
    {
        uint8_t encoded_sys_id[BLE_DIS_SYS_ID_LEN];

        sys_id_encode(encoded_sys_id, p_dis_init->p_sys_id);
        err_code = char_add(p_dis, 
                            BLE_UUID_SYSTEM_ID_CHAR,
                            encoded_sys_id,
                            BLE_DIS_SYS_ID_LEN,
                            &p_dis_init->dis_attr_md,
                            &p_dis->sys_id_handles);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    if (p_dis_init->p_reg_cert_data_list != NULL)
    {
        err_code = char_add(p_dis, 
                            BLE_UUID_IEEE_REGULATORY_CERTIFICATION_DATA_LIST_CHAR,
                            p_dis_init->p_reg_cert_data_list->p_list,
                            p_dis_init->p_reg_cert_data_list->list_len,
                            &p_dis_init->dis_attr_md,
                            &p_dis->reg_cert_data_list_handles);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    if (p_dis_init->p_pnp_id != NULL)
    {
        uint8_t encoded_pnp_id[BLE_DIS_PNP_ID_LEN];

        pnp_id_encode(encoded_pnp_id, p_dis_init->p_pnp_id);
        err_code = char_add(p_dis, 
                            BLE_UUID_PNP_ID_CHAR,
                            encoded_pnp_id,
                            BLE_DIS_PNP_ID_LEN,
                            &p_dis_init->dis_attr_md,
                            &p_dis->pnp_id_handles);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    
    // Initialize service structure
    p_dis->evt_handler = p_dis_init->evt_handler;
    p_dis->conn_handle = BLE_CONN_HANDLE_INVALID;
    p_dis->temp_type   = p_dis_init->temp_type;

    
    // Add measurement characteristic
    err_code = dis_measurement_char_add(p_dis, p_dis_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }


    return NRF_SUCCESS;
}


uint32_t ble_dis_measurement_send(ble_dis_t * p_dis, ble_dis_meas_t * p_dis_meas)
{
    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;
  
    uint8_t                encoded_dis_meas[MAX_DIM_LEN];
    uint16_t               len;
  
    len     = dis_measurement_encode(p_dis, p_dis_meas, encoded_dis_meas);

      // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = len;
    gatts_value.offset  = 0;
    gatts_value.p_value = encoded_dis_meas;

    // Update database.
    err_code = sd_ble_gatts_value_set(p_dis->conn_handle,
                                      p_dis->meas_handles.value_handle,
                                      &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
  

    // Send value if connected
    if (p_dis->conn_handle != BLE_CONN_HANDLE_INVALID && p_dis->is_meas_notification_enabled)
    {

        uint16_t               hvx_len;
        ble_gatts_hvx_params_t hvx_params;


        hvx_len = len;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_dis->meas_handles.value_handle;
//        hvx_params.type   = BLE_GATT_HVX_INDICATION;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &hvx_len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(p_dis->conn_handle, &hvx_params);
        if ((err_code == NRF_SUCCESS) && (hvx_len != len))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}


uint32_t ble_dis_is_indication_enabled(ble_dis_t * p_dis, bool * p_indication_enabled)
{
    uint32_t err_code;
    uint8_t  cccd_value_buf[BLE_CCCD_VALUE_LEN];
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = BLE_CCCD_VALUE_LEN;
    gatts_value.offset  = 0;
    gatts_value.p_value = cccd_value_buf;

    err_code = sd_ble_gatts_value_get(p_dis->conn_handle,
                                      p_dis->meas_handles.cccd_handle,
                                      &gatts_value);
    if (err_code == NRF_SUCCESS)
    {
        *p_indication_enabled = ble_srv_is_indication_enabled(cccd_value_buf);
    }
    if (err_code == BLE_ERROR_GATTS_SYS_ATTR_MISSING)
    {
        *p_indication_enabled = false;
        return NRF_SUCCESS;
    }
    return err_code;
}

#endif // NRF_MODULE_ENABLED(BLE_DIS)
