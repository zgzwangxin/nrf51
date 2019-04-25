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

/** @file
 *
 * @defgroup ble_dis Device Information Service
 * @{
 * @ingroup ble_sdk_srv
 * @brief Device Information Service module.
 *
 * @details This module implements the Device Information Service.
 *          During initialization it adds the Device Information Service to the BLE stack database.
 *          It then encodes the supplied information, and adds the curresponding characteristics.
 *
 * @note Attention!
 *  To maintain compliance with Nordic Semiconductor ASA Bluetooth profile
 *  qualification listings, this section of source code must not be modified.
 */

#ifndef BLE_DIS_H__
#define BLE_DIS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_date_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup DIS_VENDOR_ID_SRC_VALUES Vendor ID Source values
 * @{
 */
#define BLE_DIS_VENDOR_ID_SRC_BLUETOOTH_SIG   1                 /**< Vendor ID assigned by Bluetooth SIG. */
#define BLE_DIS_VENDOR_ID_SRC_USB_IMPL_FORUM  2                 /**< Vendor ID assigned by USB Implementer's Forum. */
/** @} */

/**@brief System ID parameters */
typedef struct
{
    uint64_t manufacturer_id;                                   /**< Manufacturer ID. Only 5 LSOs shall be used. */
    uint32_t organizationally_unique_id;                        /**< Organizationally unique ID. Only 3 LSOs shall be used. */
} ble_dis_sys_id_t;

/**@brief IEEE 11073-20601 Regulatory Certification Data List Structure */
typedef struct
{
    uint8_t *  p_list;                                          /**< Pointer the byte array containing the encoded opaque structure based on IEEE 11073-20601 specification. */
    uint8_t    list_len;                                        /**< Length of the byte array. */
} ble_dis_reg_cert_data_list_t;

/**@brief PnP ID parameters */
typedef struct
{
    uint8_t  vendor_id_source;                                  /**< Vendor ID Source. see @ref DIS_VENDOR_ID_SRC_VALUES. */
    uint16_t vendor_id;                                         /**< Vendor ID. */
    uint16_t product_id;                                        /**< Product ID. */
    uint16_t product_version;                                   /**< Product Version. */
} ble_dis_pnp_id_t;






/**@brief Health Thermometer Service event type. */
typedef enum
{
    BLE_DIS_EVT_INDICATION_ENABLED,                                         /**< Health Thermometer value indication enabled event. */
    BLE_DIS_EVT_INDICATION_DISABLED,                                        /**< Health Thermometer value indication disabled event. */
    BLE_DIS_EVT_INDICATION_CONFIRMED                                        /**< Confirmation of a temperature measurement indication has been received. */
} ble_dis_evt_type_t;

/**@brief Health Thermometer Service event. */
typedef struct
{
    ble_dis_evt_type_t evt_type;                                            /**< Type of event. */
} ble_dis_evt_t;

// Forward declaration of the ble_hts_t type.
typedef struct ble_dis_s ble_dis_t;

/**@brief Health Thermometer Service event handler type. */
typedef void (*ble_dis_evt_handler_t) (ble_dis_t * p_dis, ble_dis_evt_t * p_evt);

/**@brief FLOAT format (IEEE-11073 32-bit FLOAT, defined as a 32-bit value with a 24-bit mantissa
 *        and an 8-bit exponent. */
typedef struct
{
  int8_t  exponent;                                                         /**< Base 10 exponent */
  int32_t mantissa;                                                         /**< Mantissa, should be using only 24 bits */
} ieee_float32_t;

/**@brief Health Thermometer Service structure. This contains various status information for
 *        the service. */
struct ble_dis_s
{
    // 设备信息
    ble_gatts_char_handles_t manufact_name_handles;
    ble_gatts_char_handles_t model_num_handles;
    ble_gatts_char_handles_t serial_num_handles;
    ble_gatts_char_handles_t hw_rev_handles;
    ble_gatts_char_handles_t fw_rev_handles;
    ble_gatts_char_handles_t sw_rev_handles;
    ble_gatts_char_handles_t sys_id_handles;
    ble_gatts_char_handles_t reg_cert_data_list_handles;
    ble_gatts_char_handles_t pnp_id_handles;
    
    // 下面与温度相关
    ble_dis_evt_handler_t        evt_handler;                               /**< Event handler to be called for handling events in the Health Thermometer Service. */
    uint16_t                     service_handle;                            /**< Handle of Health Thermometer Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t     meas_handles;                              /**< Handles related to the Health Thermometer Measurement characteristic. */
    ble_gatts_char_handles_t     temp_type_handles;                         /**< Handles related to the Health Thermometer Temperature Type characteristic. */
    uint16_t                     conn_handle;                               /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                      temp_type;                                 /**< Temperature type indicates where the measurement was taken. */
};

/**@brief Health Thermometer Service measurement structure. This contains a Health Thermometer
 *        measurement. */
typedef struct ble_dis_meas_s
{
    bool                         temp_in_fahr_units;                        /**< True if Temperature is in Fahrenheit units, Celcius otherwise. */
    bool                         time_stamp_present;                        /**< True if Time Stamp is present. */
    bool                         temp_type_present;                         /**< True if Temperature Type is present. */
    ieee_float32_t               temp_in_celcius;                           /**< Temperature Measurement Value (Celcius). */
    ieee_float32_t               temp_in_fahr;                              /**< Temperature Measurement Value (Fahrenheit). */
    ble_date_time_t              time_stamp;                                /**< Time Stamp. */
    uint8_t                      temp_type;                                 /**< Temperature Type. */
} ble_dis_meas_t;



/**@brief Device Information Service init structure. This contains all possible characteristics
 *        needed for initialization of the service.
 */
typedef struct
{
    ble_srv_utf8_str_t             manufact_name_str;           /**< Manufacturer Name String. */
    ble_srv_utf8_str_t             model_num_str;               /**< Model Number String. */
    ble_srv_utf8_str_t             serial_num_str;              /**< Serial Number String. */
    ble_srv_utf8_str_t             hw_rev_str;                  /**< Hardware Revision String. */
    ble_srv_utf8_str_t             fw_rev_str;                  /**< Firmware Revision String. */
    ble_srv_utf8_str_t             sw_rev_str;                  /**< Software Revision String. */
    ble_dis_sys_id_t *             p_sys_id;                    /**< System ID. */
    ble_dis_reg_cert_data_list_t * p_reg_cert_data_list;        /**< IEEE 11073-20601 Regulatory Certification Data List. */
    ble_dis_pnp_id_t *             p_pnp_id;                    /**< PnP ID. */
    ble_srv_security_mode_t        dis_attr_md;                 /**< Initial Security Setting for Device Information Characteristics. */

    ble_dis_evt_handler_t        evt_handler;                               /**< Event handler to be called for handling events in the Health Thermometer Service. */
    ble_srv_cccd_security_mode_t dis_meas_attr_md;                          /**< Initial security level for health thermometer measurement attribute */
    ble_srv_security_mode_t      dis_temp_type_attr_md;                     /**< Initial security level for health thermometer tempearture type attribute */
    uint8_t                      temp_type_as_characteristic;               /**< Set non-zero if temp type given as characteristic */
    uint8_t                      temp_type;                                 /**< Temperature type if temperature characteristic is used */


} ble_dis_init_t;





/**@brief Function for initializing the Device Information Service.
 *
 * @details This call allows the application to initialize the device information service.
 *          It adds the DIS service and DIS characteristics to the database, using the initial
 *          values supplied through the p_dis_init parameter. Characteristics which are not to be
 *          added, shall be set to NULL in p_dis_init.
 *
 * @param[in]   p_dis_init   The structure containing the values of characteristics needed by the
 *                           service.
 *
 * @return      NRF_SUCCESS on successful initialization of service.
 */
uint32_t ble_dis_init(ble_dis_t * p_dis, const ble_dis_init_t * p_dis_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Health Thermometer Service.
 *
 * @param[in]   p_dis      Health Thermometer Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_dis_on_ble_evt(ble_dis_t * p_dis, ble_evt_t * p_ble_evt);

/**@brief Function for sending health thermometer measurement if indication has been enabled.
 *
 * @details The application calls this function after having performed a Health Thermometer
 *          measurement. If indication has been enabled, the measurement data is encoded and
 *          sent to the client.
 *
 * @param[in]   p_hts       Health Thermometer Service structure.
 * @param[in]   p_hts_meas  Pointer to new health thermometer measurement.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_dis_measurement_send(ble_dis_t * p_dis, ble_dis_meas_t * p_dis_meas);

/**@brief Function for checking if indication of Temperature Measurement is currently enabled.
 *
 * @param[in]   p_hts                  Health Thermometer Service structure.
 * @param[out]  p_indication_enabled   TRUE if indication is enabled, FALSE otherwise.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_dis_is_indication_enabled(ble_dis_t * p_dis, bool * p_indication_enabled);


#ifdef __cplusplus
}
#endif

#endif // BLE_DIS_H__

/** @} */
