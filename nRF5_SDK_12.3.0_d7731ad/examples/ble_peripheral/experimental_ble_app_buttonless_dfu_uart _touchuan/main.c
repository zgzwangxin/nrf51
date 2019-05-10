/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
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
 * @defgroup ble_sdk_app_template_main main.c
 * @{
 * @ingroup ble_sdk_app_template
 * @brief Template project main file.
 *
 * This file contains a template for creating a new application. It has the code necessary to wakeup
 * from button, advertise, get a connection restart advertising on disconnect and if no new
 * connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified
 * with 'YOUR_JOB' indicates where and how you can customize.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "boards.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "fstorage.h"
#include "fds.h"
#include "peer_manager.h"

#include "bsp.h"
#include "bsp_btn_ble.h"
#include "sensorsim.h"
#include "nrf_gpio.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"

#include "ble_dfu.h"
#include "ble_bas.h"
#include "ble_dis_temp.h"
//#include "ble_hts.h"

#include "ble_nus.h"
#include "app_uart.h"
#include <string.h>

#include "nrf_drv_wdt.h"

#include "nrf_delay.h"

#include "battery_voltage_level.h"
//#include "finger_mark.h"
//#include "open_door.h"

#include "global.h"

#include "nrf_uart.h"

#include "lin.h"
 

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT 1                                           /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define DEVICE_NAME                     "Nordic"                                    /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "XD"                                        /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                1600                                        /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      0                                           /**< The advertising timeout in units of seconds. */  // wx180502 180 改为 0 总是广播

//#define DEVICE_NAME                     "Nordic_BPS"                                /**< Name of device. Will be included in the advertising data. */
//#define MANUFACTURER_NAME               "NordicSemiconductor"                       /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUM                       "NS-BPS-EXAMPLE"                            /**< Model number. Will be passed to Device Information Service. */
#define MANUFACTURER_ID                 0x1122334455                                /**< Manufacturer ID, part of System ID. Will be passed to Device Information Service. */
#define ORG_UNIQUE_ID                   0x667788                                    /**< Organizational Unique ID, part of System ID. Will be passed to Device Information Service. */
#define SOFTWARE_REVISION               "2019.01.28:0003#"

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         5                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define STATIC_PASSKEY "123456"
static ble_opt_t m_static_pin_option;

//#define LED_PIN_NUMBER              21


#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                            /**< Handle of the current connection. */

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */

#define USE_DFU 0

#if USE_DFU
static ble_dfu_t m_dfus;                                                            /**< Structure used to identify the DFU service. */

static void ble_dfu_evt_handler(ble_dfu_t * p_dfu, ble_dfu_evt_t * p_evt)
{
    switch (p_evt->type)
    {
        case BLE_DFU_EVT_INDICATION_DISABLED:
            NRF_LOG_INFO("Indication for BLE_DFU is disabled\r\n");
            break;

        case BLE_DFU_EVT_INDICATION_ENABLED:
            NRF_LOG_INFO("Indication for BLE_DFU is enabled\r\n");
            break;

        case BLE_DFU_EVT_ENTERING_BOOTLOADER:
            NRF_LOG_INFO("Device is entering bootloader mode!\r\n");
            break;
        default:
            NRF_LOG_INFO("Unknown event from ble_dfu\r\n");
            break;
    }
}
#endif

static ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */

static ble_bas_t                        m_bas;

//static ble_hts_t                        m_hts;                                       /**< Structure used to identify the health thermometer service. */

static ble_dis_t                        m_dis;

//static bool              m_dis_meas_ind_conf_pending = false; /**< Flag to keep track of when an indication confirmation is pending. */

//static sensorsim_cfg_t   m_temp_celcius_sim_cfg;              /**< Temperature simulator configuration. */
//static sensorsim_state_t m_temp_celcius_sim_state;            /**< Temperature simulator state. */

#define MIN_CELCIUS_DEGREES             3688                                        /**< Minimum temperature in celcius for use in the simulated measurement function (multiplied by 100 to avoid floating point arithmetic). */
#define MAX_CELCIUS_DEGRESS             3972                                        /**< Maximum temperature in celcius for use in the simulated measurement function (multiplied by 100 to avoid floating point arithmetic). */
#define CELCIUS_DEGREES_INCREMENT       36                                          /**< Value by which temperature is incremented/decremented for each call to the simulated measurement function (multiplied by 100 to avoid floating point arithmetic). */


//APP_TIMER_DEF(m_open_cmd_timer_id);                  /**< open cmd timer. */
APP_TIMER_DEF(m_soft_timer1_timer_id);               /**< timer. */

//static bool is_adving = false;

// YOUR_JOB: Use UUIDs for service(s) used in your application.
static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}}; /**< Universally unique service identifiers. */

static void advertising_start(void);

void send_string(uint8_t * p_string, uint16_t length);
void send_string_tx(uint8_t * p_string, uint16_t length);
#define STRING_LEN  18
extern char string[STRING_LEN];


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            NRF_LOG_DEBUG("Connected to previously bonded device\r\n");
            err_code = pm_peer_rank_highest(p_evt->peer_id);
            if (err_code != NRF_ERROR_BUSY)
            {
                APP_ERROR_CHECK(err_code);
            }
        } break; // PM_EVT_BONDED_PEER_CONNECTED

        case PM_EVT_CONN_SEC_START:
            break; // PM_EVT_CONN_SEC_START

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            NRF_LOG_DEBUG("Link secured. Role: %d. conn_handle: %d, Procedure: %d\r\n",
                                 ble_conn_state_role(p_evt->conn_handle),
                                 p_evt->conn_handle,
                                 p_evt->params.conn_sec_succeeded.procedure);
            err_code = pm_peer_rank_highest(p_evt->peer_id);
            if (err_code != NRF_ERROR_BUSY)
            {
                APP_ERROR_CHECK(err_code);
            }
        } break; // PM_EVT_CONN_SEC_SUCCEEDED

        case PM_EVT_CONN_SEC_FAILED:
        {
            /** Often, when securing fails, it shouldn't be restarted, for security reasons.
             *  Other times, it can be restarted directly.
             *  Sometimes it can be restarted, but only after changing some Security Parameters.
             *  Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             *  Sometimes it is impossible, to secure the link, or the peer device does not support it.
             *  How to handle this error is highly application dependent. */
        } break; // PM_EVT_CONN_SEC_FAILED

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // not Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break; // PM_EVT_CONN_SEC_CONFIG_REQ

        case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            err_code = fds_gc();
            if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }
        } break; // PM_EVT_STORAGE_FULL

        case PM_EVT_ERROR_UNEXPECTED:
            // Assert.
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
            break; // PM_EVT_ERROR_UNEXPECTED

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
            break; // PM_EVT_PEER_DATA_UPDATE_SUCCEEDED

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
            // Assert.
            APP_ERROR_CHECK_BOOL(false);
            break; // PM_EVT_PEER_DATA_UPDATE_FAILED

        case PM_EVT_PEER_DELETE_SUCCEEDED:
            break; // PM_EVT_PEER_DELETE_SUCCEEDED

        case PM_EVT_PEER_DELETE_FAILED:
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
            break; // PM_EVT_PEER_DELETE_FAILED

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            advertising_start();
            break; // PM_EVT_PEERS_DELETE_SUCCEEDED

        case PM_EVT_PEERS_DELETE_FAILED:
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
            break; // PM_EVT_PEERS_DELETE_FAILED

        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
            break; // PM_EVT_LOCAL_DB_CACHE_APPLIED

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
            break; // PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED

        case PM_EVT_SERVICE_CHANGED_IND_SENT:
            break; // PM_EVT_SERVICE_CHANGED_IND_SENT

        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
            break; // PM_EVT_SERVICE_CHANGED_IND_SENT

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Handle events from open_cmd timer.
 *
 * @param[in]   p_context   parameter registered in timer start function.
 */
//static void open_cmd_timer_handler(void * p_context)
//{
//    UNUSED_PARAMETER(p_context);
//	// 关闭先前打开的 led2 pin20
//	
//	nrf_gpio_pin_write(LED_PIN_NUMBER, LEDS_ACTIVE_STATE ? 0 : 1);
//    	
////	nrf_gpio_pin_write(0, LEDS_ACTIVE_STATE ? 0 : 1);
//}

//void open_cmd_start(void)
//{
//	uint32_t err_code = NRF_SUCCESS;

//	// 打开 led2 pin20
////	bsp_board_led_on(LED_1);
//	nrf_gpio_cfg_output(LED_PIN_NUMBER);
//	nrf_gpio_pin_write(LED_PIN_NUMBER, LEDS_ACTIVE_STATE ? 1 : 0);
////    nrf_gpio_cfg_output(0);
////	nrf_gpio_pin_write(0, LEDS_ACTIVE_STATE ? 1 : 0);
//	
//	err_code = app_timer_start(m_open_cmd_timer_id, APP_TIMER_TICKS(2000, APP_TIMER_PRESCALER), NULL);
//	APP_ERROR_CHECK(err_code);
//}


/**@brief Handle events from open_cmd timer.
 *
 * @param[in]   p_context   parameter registered in timer start function.
 */
static void soft_timer1_timer_handler(void * p_context)
{
//	uint32_t err_code = NRF_SUCCESS;

    UNUSED_PARAMETER(p_context);
    
//    nrf_gpio_pin_write(16, LEDS_ACTIVE_STATE ? 1 : 0);
//    
//    battery_voltage_check_state(&g_battery_voltage_manage);

//    nrf_gpio_pin_write(16, LEDS_ACTIVE_STATE ? 0 : 1);
  
  Lin_master_go();
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    uint32_t err_code = NRF_SUCCESS;

    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create timers.

//	err_code = app_timer_create(&m_open_cmd_timer_id,
//							APP_TIMER_MODE_SINGLE_SHOT,
//							open_cmd_timer_handler);
//	APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_create(&m_soft_timer1_timer_id,
							APP_TIMER_MODE_REPEATED,
							soft_timer1_timer_handler);
	APP_ERROR_CHECK(err_code);
	
  
    /* YOUR_JOB: Create any timers to be used by the application.
                 Below is an example of how to create a timer.
                 For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by
                 one.
       uint32_t err_code;
       err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
       APP_ERROR_CHECK(err_code); */
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
                                          
    uint8_t passkey[] = STATIC_PASSKEY;                                                     
    m_static_pin_option.gap_opt.passkey.p_passkey = passkey;
    err_code =  sd_ble_opt_set(BLE_GAP_OPT_PASSKEY, &m_static_pin_option);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the YYY Service events.
 * YOUR_JOB implement a service handler function depending on the event the service you are using can generate
 *
 * @details This function will be called for all YY Service events which are passed to
 *          the application.
 *
 * @param[in]   p_yy_service   YY Service structure.
 * @param[in]   p_evt          Event received from the YY Service.
 *
 *
   static void on_yys_evt(ble_yy_service_t     * p_yy_service,
                       ble_yy_service_evt_t * p_evt)
   {
    switch (p_evt->evt_type)
    {
        case BLE_YY_NAME_EVT_WRITE:
            APPL_LOG("[APPL]: charact written with value %s. \r\n", p_evt->params.char_xx.value.p_str);
            break;

        default:
            // No implementation needed.
            break;
    }
   }*/


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_nus    Nordic UART Service structure.
 * @param[in] p_data   Data to be send to UART module.
 * @param[in] length   Length of the data.
 */
/**@snippet [Handling the data received over BLE] */
static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
  uint8_t len, ID;
  
  if (p_data[0] == 0xaa && Lin_CheckPID(p_data[1])) {
    
    ID = p_data[1] & 0x3f;
    len = 0x01 << (((ID >> 4) & 0x03) + 1);
    
    if (Lin_Check_Sum(p_data + 2, len) == p_data[len + 2]) {
      
      Lin_ID_data_press(ID, p_data + 2);
    }
  }
  
//    for (uint32_t i = 0; i < length; i++)
//    {
//        while (app_uart_put(p_data[i]) != NRF_SUCCESS);
//    }
//    while (app_uart_put('\r') != NRF_SUCCESS);
//    while (app_uart_put('\n') != NRF_SUCCESS);
}
/**@snippet [Handling the data received over BLE] */


#define TEMP_TYPE_AS_CHARACTERISTIC     0                                           /**< Determines if temperature type is given as characteristic (1) or as a field of measurement (0). */

///**@brief Function for populating simulated health thermometer measurement.
// */
//static void dis_sim_measurement(ble_dis_meas_t * p_meas)
//{
//    static ble_date_time_t time_stamp = { 2012, 12, 5, 11, 50, 0 };

//    uint32_t celciusX100;

//    p_meas->temp_in_fahr_units = false;
//    p_meas->time_stamp_present = true;
//    p_meas->temp_type_present  = (TEMP_TYPE_AS_CHARACTERISTIC ? false : true);

//    celciusX100 = sensorsim_measure(&m_temp_celcius_sim_state, &m_temp_celcius_sim_cfg);

//    p_meas->temp_in_celcius.exponent = -2;
//    p_meas->temp_in_celcius.mantissa = celciusX100;
//    p_meas->temp_in_fahr.exponent    = -2;
//    p_meas->temp_in_fahr.mantissa    = (32 * 100) + ((celciusX100 * 9) / 5);
//    p_meas->time_stamp               = time_stamp;
//    p_meas->temp_type                = 0;

//    // update simulated time stamp
//    time_stamp.seconds += 27;
//    if (time_stamp.seconds > 59)
//    {
//        time_stamp.seconds -= 60;
//        time_stamp.minutes++;
//        if (time_stamp.minutes > 59)
//        {
//            time_stamp.minutes = 0;
//        }
//    }
//}

static void dis_temp_measurement(ble_dis_meas_t * p_meas, float temp)
{
    static ble_date_time_t time_stamp = { 2012, 12, 5, 11, 50, 0 };

//    uint32_t celciusX100;

    p_meas->temp_in_fahr_units = false;
    p_meas->time_stamp_present = false;
    p_meas->temp_type_present  = (TEMP_TYPE_AS_CHARACTERISTIC ? false : true);

//    celciusX100 = sensorsim_measure(&m_temp_celcius_sim_state, &m_temp_celcius_sim_cfg);

    p_meas->temp_in_celcius.exponent = -2;
    p_meas->temp_in_celcius.mantissa = temp * 100;
    p_meas->temp_in_fahr.exponent    = -2;
    p_meas->temp_in_fahr.mantissa    = (32 * 100) + ((temp * 9) / 5);
    p_meas->time_stamp               = time_stamp;
    p_meas->temp_type                = 0;

}


///**@brief Function for simulating and sending one Temperature Measurement.
// */
//static void temperature_measurement_send(void)
//{
//    ble_dis_meas_t simulated_meas;
//    uint32_t       err_code;

//    if (!m_dis_meas_ind_conf_pending)
//    {
//        dis_sim_measurement(&simulated_meas);

//        err_code = ble_dis_measurement_send(&m_dis, &simulated_meas);

//        switch (err_code)
//        {
//            case NRF_SUCCESS:
//                // Measurement was successfully sent, wait for confirmation.
//                m_dis_meas_ind_conf_pending = true;
//                break;

//            case NRF_ERROR_INVALID_STATE:
//                // Ignore error.
//                break;

//            default:
//                APP_ERROR_HANDLER(err_code);
//                break;
//        }
//    }
//}


void temperature_measurement_send_temp(float temp)
{
    ble_dis_meas_t simulated_meas;
    uint32_t       err_code;
    float          temp_bak = -10000;

//    dis_sim_measurement(&simulated_meas);
    if (temp != temp_bak)
    {
        dis_temp_measurement(&simulated_meas, temp);

        err_code = ble_dis_measurement_send(&m_dis, &simulated_meas);
        UNUSED_VARIABLE(err_code);
        temp_bak = temp;
    }
}

/**@brief Function for handling the Health Thermometer Service events.
 *
 * @details This function will be called for all Health Thermometer Service events which are passed
 *          to the application.
 *
 * @param[in] p_hts  Health Thermometer Service structure.
 * @param[in] p_evt  Event received from the Health Thermometer Service.
 */
static void on_dis_evt(ble_dis_t * p_dis, ble_dis_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_DIS_EVT_INDICATION_ENABLED:
            // Indication has been enabled, send a single temperature measurement
          //  open_cmd_start();
        
//            snprintf(string, STRING_LEN, "INDICATION_EN");
//            send_string((uint8_t*)string, strlen(string));
            
//            temperature_measurement_send();
            break;

        case BLE_DIS_EVT_INDICATION_CONFIRMED:
//            m_dis_meas_ind_conf_pending = false;
            break;

        default:
            // No implementation needed.
            break;
    }
}



/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t err_code;

  #if USE_DFU
    ble_dfu_init_t dfus_init;

    // Initialize the Device Firmware Update Service.
    memset(&dfus_init, 0, sizeof(dfus_init));

    dfus_init.evt_handler                               = ble_dfu_evt_handler;
    dfus_init.ctrl_point_security_req_write_perm        = SEC_SIGNED;
    dfus_init.ctrl_point_security_req_cccd_write_perm   = SEC_SIGNED;

    err_code = ble_dfu_init(&m_dfus, &dfus_init);
    APP_ERROR_CHECK(err_code);
    /**/
  #endif
	
    ble_nus_init_t nus_init;

    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
	
    //*
    ble_bas_init_t bas_init;

    memset(&bas_init, 0, sizeof(bas_init));

    // Here the sec level for the Battery Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;

    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);
	
    
    
    ble_dis_init_t   dis_init;
    ble_dis_sys_id_t sys_id;
    
    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.model_num_str, MODEL_NUM);
    ble_srv_ascii_to_utf8(&dis_init.sw_rev_str, SOFTWARE_REVISION);
    
    sys_id.manufacturer_id            = MANUFACTURER_ID;
    sys_id.organizationally_unique_id = ORG_UNIQUE_ID;
    dis_init.p_sys_id                 = &sys_id;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);


    dis_init.evt_handler                 = on_dis_evt;
    dis_init.temp_type_as_characteristic = TEMP_TYPE_AS_CHARACTERISTIC;
    dis_init.temp_type                   = 0;

    // Here the sec level for the Health Thermometer Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_meas_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_meas_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_meas_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_temp_type_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_temp_type_attr_md.write_perm);


    err_code = ble_dis_init(&m_dis, &dis_init);
    APP_ERROR_CHECK(err_code);
    /**/
    
    
//    ble_hts_init_t   hts_init;
//    
//    // Initialize Health Thermometer Service
//    memset(&hts_init, 0, sizeof(hts_init));

//    hts_init.evt_handler                 = NULL;
//    hts_init.temp_type_as_characteristic = TEMP_TYPE_AS_CHARACTERISTIC;
//    hts_init.temp_type                   = BLE_HTS_TEMP_TYPE_BODY;

//    // Here the sec level for the Health Thermometer Service can be changed/increased.
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&hts_init.hts_meas_attr_md.cccd_write_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&hts_init.hts_meas_attr_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hts_init.hts_meas_attr_md.write_perm);

//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&hts_init.hts_temp_type_attr_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hts_init.hts_temp_type_attr_md.write_perm);

//    err_code = ble_hts_init(&m_hts, &hts_init);
//    APP_ERROR_CHECK(err_code);

    
    
    
    /* YOUR_JOB: Add code to initialize the services used by the application.
       uint32_t                           err_code;
       ble_xxs_init_t                     xxs_init;
       ble_yys_init_t                     yys_init;

       // Initialize XXX Service.
       memset(&xxs_init, 0, sizeof(xxs_init));

       xxs_init.evt_handler                = NULL;
       xxs_init.is_xxx_notify_supported    = true;
       xxs_init.ble_xx_initial_value.level = 100;

       err_code = ble_bas_init(&m_xxs, &xxs_init);
       APP_ERROR_CHECK(err_code);

       // Initialize YYY Service.
       memset(&yys_init, 0, sizeof(yys_init));
       yys_init.evt_handler                  = on_yys_evt;
       yys_init.ble_yy_initial_value.counter = 0;

       err_code = ble_yy_service_init(&yys_init, &yy_init);
       APP_ERROR_CHECK(err_code);
     */
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting timers.
 */
static void application_timers_start(void)
{
    uint32_t               err_code;
    
    /* YOUR_JOB: Start your timers. below is an example of how to start a timer.
       uint32_t err_code;
       err_code = app_timer_start(m_app_timer_id, TIMER_INTERVAL, NULL);
       APP_ERROR_CHECK(err_code); */

    err_code = app_timer_start(m_soft_timer1_timer_id, APP_TIMER_TICKS(TIMER1_CIRCLE_MS, APP_TIMER_PRESCALER), NULL);
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t err_code;
    
//    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
//    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
//    err_code = bsp_btn_ble_sleep_mode_prepare();
//    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
//            is_adving = true;
        
//            nrf_gpio_pin_write(22, LEDS_ACTIVE_STATE ? 1 : 0);
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;

        default:
            break;
    }
}


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
//            err_code = bsp_indication_set(BSP_INDICATE_IDLE);
//            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GAP_EVT_CONNECTED:
//            is_adving = false;
        
//            nrf_gpio_pin_write(22, LEDS_ACTIVE_STATE ? 0 : 1);
        
            nrf_gpio_pin_write(0, LEDS_ACTIVE_STATE ? 0 : 1);
            nrf_gpio_pin_write(20, LEDS_ACTIVE_STATE ? 0 : 1);
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        
         //   nrf_gpio_pin_write(0, LEDS_ACTIVE_STATE ? 0 : 1);
        
            // 1连接一建立就发送安全请求，从而促使手机发送配对请求过来
//            ble_gap_sec_params_t sec_param;
//            memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

//            // Security parameters to be used for all security procedures.
//            sec_param.bond           = SEC_PARAM_BOND;
//            sec_param.mitm           = SEC_PARAM_MITM;
//            sec_param.lesc           = SEC_PARAM_LESC;
//            sec_param.keypress       = SEC_PARAM_KEYPRESS;
//            sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
//            sec_param.oob            = SEC_PARAM_OOB;
//            sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
//            sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
//            sec_param.kdist_own.enc  = 1;
//            sec_param.kdist_own.id   = 1;
//            sec_param.kdist_peer.enc = 1;
//            sec_param.kdist_peer.id  = 1;
//            err_code = sd_ble_gap_authenticate(m_conn_handle, &sec_param);
//            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_TIMEOUT

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_EVT_USER_MEM_REQUEST

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif
           // 2回复配对请求
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        {
        //   resp_pair_request();
//            ble_gap_sec_params_t sec_params;
//            uint32_t                    err_code;

//            memset(&sec_params,0,sizeof(ble_gap_sec_params_t));

//            // Security parameters to be used for all security procedures.
//            sec_param.bond           = SEC_PARAM_BOND;
//            sec_param.mitm           = SEC_PARAM_MITM;
//            sec_param.lesc           = SEC_PARAM_LESC;
//            sec_param.keypress       = SEC_PARAM_KEYPRESS;
//            sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
//            sec_param.oob            = SEC_PARAM_OOB;
//            sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
//            sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
//            sec_param.kdist_own.enc  = 1;
//            sec_param.kdist_own.id   = 1;
//            sec_param.kdist_peer.enc = 1;
//            sec_param.kdist_peer.id  = 1;

//            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_SUCCESS, &sec_params, NULL);
//            APP_ERROR_CHECK(err_code);
        } break;

        // 3判断配对是否成功，如果不成功断开连接，从而阻止他人任意连接。
        case BLE_GAP_EVT_AUTH_STATUS:
          if (p_ble_evt->evt.gap_evt.params.auth_status.auth_status == BLE_GAP_SEC_STATUS_SUCCESS) {
          //  printf("pair success\r\n");
          } else {
            sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
          }

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
                
//    nrf_gpio_pin_write(16, LEDS_ACTIVE_STATE ? 1 : 0);
    
    /** The Connection state module has to be fed BLE events in order to function correctly
     * Remember to call ble_conn_state_on_ble_evt before calling any ble_conns_state_* functions. */
    ble_conn_state_on_ble_evt(p_ble_evt);
    pm_on_ble_evt(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
  #if USE_DFU
    ble_dfu_on_ble_evt(&m_dfus, p_ble_evt);
  #endif
    ble_bas_on_ble_evt(&m_bas,  p_ble_evt);
	
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    
    ble_dis_on_ble_evt(&m_dis, p_ble_evt);
    
//    nrf_gpio_pin_write(16, LEDS_ACTIVE_STATE ? 0 : 1);
    /*YOUR_JOB add calls to _on_ble_evt functions from each service your application is using
       ble_xxs_on_ble_evt(&m_xxs, p_ble_evt);
       ble_yys_on_ble_evt(&m_yys, p_ble_evt);
     */
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
//    nrf_gpio_pin_write(16, LEDS_ACTIVE_STATE ? 1 : 0);
    
    // Dispatch the system event to the fstorage module, where it will be
    // dispatched to the Flash Data Storage (FDS) module.
    fs_sys_event_handler(sys_evt);

    // Dispatch to the Advertising module last, since it will check if there are any
    // pending flash operations in fstorage. Let fstorage process system events first,
    // so that it can report correctly to the Advertising module.
    ble_advertising_on_sys_evt(sys_evt);
    
    
//    nrf_gpio_pin_write(16, LEDS_ACTIVE_STATE ? 0 : 1);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

//    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
  
//  nrf_clock_lf_cfg_t clock_lf_cfg =       {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
//                                           .rc_ctiv       = 0,                                \
//                                           .rc_temp_ctiv  = 0,                                \
//                                           .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM};
  nrf_clock_lf_cfg_t clock_lf_cfg = 
{
    .source = NRF_CLOCK_LF_SRC_RC,
    .rc_ctiv = 16, // Interval in 0.25 s, 16 * 0.25 = 4 sec
    .rc_temp_ctiv = 2, // Check temperature every .rc_ctiv, but calibrate every .rc_temp_ctiv 
    .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM,
};

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);
//    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_250MS_CALIBRATION, NULL); 

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
static void peer_manager_init(bool erase_bonds)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    if (erase_bonds)
    {
        err_code = pm_peers_delete();
        APP_ERROR_CHECK(err_code);
    }

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
static void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break; // BSP_EVENT_SLEEP

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break; // BSP_EVENT_DISCONNECT

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist();
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break; // BSP_EVENT_KEY_0

        default:
            break;
    }
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advdata_t          advdata;
    ble_adv_modes_config_t options;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    memset(&options, 0, sizeof(options));
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LED /* | BSP_INIT_BUTTONS */,
                                 APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                                 bsp_event_handler);

    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();

    APP_ERROR_CHECK(err_code);
}

/**@brief  Function for initializing the UART module.
 */

void uart_init(void);

/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' i.e '\r\n' (hex 0x0D) or if the string has reached a length of
 *          @ref NUS_MAX_DATA_LENGTH.
 */
/**@snippet [Handling the data received over UART] */
void uart_event_handle(app_uart_evt_t * p_event)
{
//    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
//    static uint8_t index = 0;
  uint8_t data;
//    uint32_t       err_code;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
//            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
//            index++;
            UNUSED_VARIABLE(app_uart_get(&data));
        
            Lin_data_ready(data);

//            if ((data_array[index - 1] == '\n') || (index >= (BLE_NUS_MAX_DATA_LEN)))
//            {
////                err_code = ble_nus_string_send(&m_nus, data_array, index);
////                if (err_code != NRF_ERROR_INVALID_STATE)
////                {
////                    APP_ERROR_CHECK(err_code);
////                }
//                err_code = ble_nus_string_send(&m_nus, data_array, index);
//                if (err_code != NRF_ERROR_INVALID_STATE)
//                {
//                    APP_ERROR_CHECK(err_code);
//                }

//                index = 0;
//            }
            break;

          case APP_UART_COMMUNICATION_ERROR:
          case APP_UART_FIFO_ERROR:
              
              nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_TXDRDY);
              nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXTO);
              nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_ERROR);
//            
              app_uart_close();
              uart_init();
            
//        case APP_UART_COMMUNICATION_ERROR:
//        
//            APP_ERROR_HANDLER(p_event->data.error_communication);
//            break;

//        case APP_UART_FIFO_ERROR:
//            APP_ERROR_HANDLER(p_event->data.error_code);
//            break;
          case APP_UART_TX_EMPTY:
              Lin_data_tx_done();
              break;

        default:
            break;
    }
}
/**@snippet [Handling the data received over UART] */

/**@snippet [UART Initialization] */
void uart_init(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,//,5
        TX_PIN_NUMBER,//,6
        RTS_PIN_NUMBER,//,7
        CTS_PIN_NUMBER,//,12
//        5,6,7,12,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud19200
    };

    APP_UART_FIFO_INIT( &comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}
/**@snippet [UART Initialization] */

/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(BLE_ADV_MODE_FAST);

    APP_ERROR_CHECK(err_code);
}


//APP_TIMER_DEF(m_test_timer_id);                  /**< open cmd timer. */

//static void test_timer_handler(void * p_context)
//{
//    UNUSED_PARAMETER(p_context);
//    
////    nrf_gpio_pin_write(20, !nrf_gpio_pin_out_read(20));
////    nrf_gpio_pin_write(0, !nrf_gpio_pin_out_read(0));
//    
//    nrf_gpio_pin_toggle(0);
//    nrf_gpio_pin_toggle(20);

//	// 
//}

void send_string(uint8_t * p_string, uint16_t length)
{
    uint32_t err_code;
    
    if (length > 18)
    {
        length = 18;
    }
    
    err_code = ble_nus_string_send(&m_nus, p_string, length);
    if (err_code != NRF_ERROR_INVALID_STATE)
    {
        APP_ERROR_CHECK(err_code);
    }
}
//void send_string_tx(uint8_t * p_string, uint16_t length)
//{
//    uint32_t err_code;
//    
//    if (length > 18)
//    {
//        length = 18;
//    }
//    
//    err_code = ble_nus_string_send_tx(&m_nus, p_string, length);
//    if (err_code != NRF_ERROR_INVALID_STATE)
//    {
//        APP_ERROR_CHECK(err_code);
//    }
//}

int set_ble_battery_level(uint8_t battery_level)
{
    ble_bas_battery_level_update(&m_bas, battery_level);
    
    return 0;
}
    
/**
 * @brief WDT events handler.
 */
void wdt_event_handler(void)
{
//    bsp_board_leds_off();

    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
}


/**@brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;
    bool     erase_bonds;

    // Initialize.
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    uart_init();
    timers_init();
  
  Lin_data_init();
	
//	// 关闭LED2、3、4
//	nrf_gpio_cfg_output(LED_PIN_NUMBER);
//	nrf_gpio_pin_write(LED_PIN_NUMBER, LEDS_ACTIVE_STATE ? 0 : 1);
    
//    // 指纹模块的touch信号
//    nrf_gpio_cfg_input(FENGER_TOUCH_PIN_NUMBER, NRF_GPIO_PIN_PULLDOWN);
    
//    // 控制指纹模块的低功耗
//    nrf_gpio_cfg_output(FENGER_TOUCH_ON_PIN_NUMBER);
////	nrf_gpio_pin_write(FENGER_TOUCH_ON_PIN_NUMBER, FENGER_TOUCH_ON_ACTIVE_LEVEL ? 1 : 0);
//	nrf_gpio_pin_write(FENGER_TOUCH_ON_PIN_NUMBER, FENGER_TOUCH_ON_ACTIVE_LEVEL ? 0 : 1);
    
    // 
    nrf_gpio_cfg_output(ADC_ON_PIN_NUMBER);
	nrf_gpio_pin_write(ADC_ON_PIN_NUMBER, ADC_ON_ACTIVE_LEVEL ? 0 : 1);
    
    nrf_gpio_cfg_output(16);
    
	nrf_gpio_cfg_output(19);
	nrf_gpio_pin_write(19, LEDS_ACTIVE_STATE ? 0 : 1);
	nrf_gpio_cfg_output(20);
	nrf_gpio_pin_write(20, LEDS_ACTIVE_STATE ? 0 : 1);
    
//	nrf_gpio_cfg_output(21);
//	nrf_gpio_pin_write(21, LEDS_ACTIVE_STATE ? 0 : 1);
//	nrf_gpio_cfg_output(22);
//	nrf_gpio_pin_write(22, LEDS_ACTIVE_STATE ? 0 : 1);
//    

//	nrf_gpio_cfg_output(28);
//	nrf_gpio_pin_write(28, LEDS_ACTIVE_STATE ? 0 : 1);
//	nrf_gpio_cfg_output(29);
//	nrf_gpio_pin_write(29, LEDS_ACTIVE_STATE ? 0 : 1);

//    nrf_gpio_cfg_output(30);
//	nrf_gpio_pin_write(30, LEDS_ACTIVE_STATE ? 0 : 1);
//    nrf_gpio_cfg_output(0);
//	nrf_gpio_pin_write(0, LEDS_ACTIVE_STATE ? 0 : 1);
    
    
//    nrf_gpio_pin_write(21, LEDS_ACTIVE_STATE ? 1 : 0);
//    nrf_delay_ms(200);
//    
//    nrf_gpio_pin_write(21, LEDS_ACTIVE_STATE ? 0 : 1);
//    nrf_delay_ms(200);
//	
//    nrf_gpio_pin_write(21, LEDS_ACTIVE_STATE ? 1 : 0);
//    nrf_delay_ms(200);
//    
//    nrf_gpio_pin_write(21, LEDS_ACTIVE_STATE ? 0 : 1);
//    nrf_delay_ms(200);
//	
//    nrf_gpio_pin_write(21, LEDS_ACTIVE_STATE ? 1 : 0);
//    nrf_delay_ms(200);
//    
//    nrf_gpio_pin_write(21, LEDS_ACTIVE_STATE ? 0 : 1);
//    nrf_delay_ms(200);
	
//    open_door_manage_init(&g_open_door_manage);
    
//    finger_mark_manage_state_machine_init(&g_finger_mark_manage_state_machine);
    
    battery_voltage_init(&g_battery_voltage_manage);
        
    buttons_leds_init(&erase_bonds);
    ble_stack_init();
    peer_manager_init(erase_bonds);
    if (erase_bonds == true)
    {
        NRF_LOG_DEBUG("Bonds erased!\r\n");
    }
    
//    sensor_simulator_init();
    
//    err_code =  sd_ble_gap_tx_power_set(4);
//    APP_ERROR_CHECK(err_code);
    
    gap_params_init();
    advertising_init();
    services_init();
    NRF_LOG_DEBUG("Services are initialized\r\n");
    conn_params_init();
    NRF_LOG_DEBUG("conn params are initialized\r\n");
    // Start execution.
    application_timers_start();
    NRF_LOG_DEBUG("app timers are started\r\n");
    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("advertising is started\r\n");
    
    nrf_gpio_cfg_output(20);
	nrf_gpio_pin_write(20, LEDS_ACTIVE_STATE ? 0 : 1);

    ble_bas_battery_level_update(&m_bas, 1);
    
    
    static nrf_drv_wdt_channel_id m_channel_id;
    
    //Configure WDT.
    nrf_drv_wdt_config_t config =     {                                                                    \
        .behaviour          = NRF_WDT_BEHAVIOUR_PAUSE_SLEEP_HALT, \
        .reload_value       = 2000,                   \
        .interrupt_priority = WDT_CONFIG_IRQ_PRIORITY,                   \
    };
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
    
    
    // Enter main loop.
    for (;;)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            nrf_drv_wdt_channel_feed(m_channel_id);
            nrf_drv_wdt_feed();
            
            power_manage();
        }
    }
}


/**
 * @}
 */
