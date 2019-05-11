

#include "battery_voltage_level.h"


#include <string.h>
#include <stdio.h>

#include "nrf_gpio.h"
#include "nrf_drv_adc.h"

#include "ble_bas.h"

#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "global.h"
#include "nrf_delay.h"

#include "nrf_temp.h"

#define ADC_BUFFER_SIZE 1                                /**< Size of buffer for ADC samples.  */
static nrf_adc_value_t       adc_buffer[ADC_BUFFER_SIZE]; /**< ADC buffer. */
//static nrf_drv_adc_channel_t m_channel_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_2); /**< Channel instance. Default configuration used. */
static nrf_drv_adc_channel_t m_channel_config = 
{
    {{
        .resolution = NRF_ADC_CONFIG_RES_10BIT,
        .input      = NRF_ADC_CONFIG_SCALING_INPUT_ONE_THIRD,
        .reference  = NRF_ADC_CONFIG_REF_VBG,
        .ain        = NRF_ADC_CONFIG_INPUT_2
    }}, NULL
};

int set_ble_battery_level(uint8_t battery_level);

void temperature_measurement_send_temp(float temp);

/**
 * @brief ADC interrupt handler.
 */
static void adc_event_handler(nrf_drv_adc_evt_t const * p_event)
{
    static const    uint16_t  BUFFER_LEN = 10;
    static          uint16_t  buffer_len_cnt = 0;
    static          uint16_t  pos = 0;
    static          uint16_t  buffer[BUFFER_LEN] = {0};
    static          uint32_t  buffer_sum = 0;
    
    if (p_event->type == NRF_DRV_ADC_EVT_DONE)
    {
        uint32_t i;
        uint32_t ad_sum = 0;
        uint16_t ad_value;
        float fad_value;
        float fvoltage ;
        for (i = 0; i < p_event->data.done.size; i++)
        {
         //   NRF_LOG_INFO("Current sample value: %d\r\n", p_event->data.done.p_buffer[i]);
            
            ad_sum += p_event->data.done.p_buffer[i];
        }
        ad_value = ad_sum / p_event->data.done.size;
        
        buffer[pos] = ad_value;
//        buffer_sum += buffer[pos];
        if (++pos >= BUFFER_LEN)
        {
            pos = 0;
        }
        if (buffer_len_cnt < BUFFER_LEN)
        {
            buffer_len_cnt++;
        }
        else
        {
//            buffer_sum -= buffer[pos];
        }
        buffer_sum = 0;
        for (i = 0; i < buffer_len_cnt; i++)
        {
            buffer_sum += buffer[i];
        }
        fad_value = 1.0 * buffer_sum / buffer_len_cnt;
        
        fvoltage = fad_value * 1.2 / ((0x01 << 10) - 1) * 3 * 2;
        
        fvoltage  =  (fvoltage - 3.0) / (4.2 - 3.0) * 100;
        
        nrf_gpio_pin_write(ADC_ON_PIN_NUMBER, ADC_ON_ACTIVE_LEVEL ? 0 : 1);
        
        set_ble_battery_level(fvoltage);
    }
}

/**
 * @brief ADC initialization.
 */
static void adc_config(void)
{
    ret_code_t ret_code;
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;

    ret_code = nrf_drv_adc_init(&config, adc_event_handler);
    APP_ERROR_CHECK(ret_code);

    nrf_drv_adc_channel_enable(&m_channel_config);
}


battery_voltage_manage_t    g_battery_voltage_manage;

 
int battery_voltage_init(battery_voltage_manage_t *p_battery_voltage_manage)
{
    if (p_battery_voltage_manage == NULL)
    {
        return 1;
    }
    p_battery_voltage_manage->state =   BATTERY_VOLTAGE_START;
    
    p_battery_voltage_manage->data.restart_wait_timeout_cnt =   0;
    p_battery_voltage_manage->data.restart_wait_timeout_N   =   MS_TO_CNT(10 * 1000, TIMER1_CIRCLE_MS);   // 10 S
    
    // 开启ADC
    adc_config();
    
    // 温度传感器初始化
    nrf_temp_init();
    
    nrf_gpio_cfg_output(ADC_ON_PIN_NUMBER);

    return 0;
}


int battery_voltage_check_state(battery_voltage_manage_t *p_battery_voltage_manage)
{
    if (p_battery_voltage_manage == NULL)
    {
        return 1;
    }
    float temp = 0;
        
    switch (p_battery_voltage_manage->state)
    {
        case BATTERY_VOLTAGE_START:
            
            nrf_gpio_pin_write(ADC_ON_PIN_NUMBER, ADC_ON_ACTIVE_LEVEL ? 1 : 0);
        
//            // 开启ADC
//            adc_config();
        
            APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(adc_buffer,ADC_BUFFER_SIZE));
        
            p_battery_voltage_manage->data.restart_wait_timeout_cnt =   0;
        
            // 开始测温
            NRF_TEMP->TASKS_START = 1; /** Start the temperature measurement. */
        
            nrf_delay_us(100);
        
            // 等待测温完成
            while (NRF_TEMP->EVENTS_DATARDY == 0)
            {
                // Do nothing.
            }
            NRF_TEMP->EVENTS_DATARDY = 0;

            nrf_drv_adc_sample();
            
            // 测温度
            temp = (nrf_temp_read() / 4.0);

            // 停止测温
            NRF_TEMP->TASKS_STOP = 1; /** Stop the temperature measurement. */
            
            temperature_measurement_send_temp(temp);
                
            p_battery_voltage_manage->state = BATTERY_VOLTAGE_AD_RESTART_WAIT;
            
            break;
        
        case BATTERY_VOLTAGE_AD_RESTART_WAIT:
            
            p_battery_voltage_manage->data.restart_wait_timeout_cnt++;
            if (p_battery_voltage_manage->data.restart_wait_timeout_cnt >=
                p_battery_voltage_manage->data.restart_wait_timeout_N
            )
            {
                p_battery_voltage_manage->data.restart_wait_timeout_cnt = 0;
                
                p_battery_voltage_manage->state = BATTERY_VOLTAGE_START;
                
//                nrf_gpio_pin_write(ADC_ON_PIN_NUMBER, ADC_ON_ACTIVE_LEVEL ? 0 : 1);
                
//                // ADC 外设关闭
//                nrf_drv_adc_uninit();
            }
        
            break;
    
        default:
            break;
    }
    
    return 0;
}
