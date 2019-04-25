/*

 */

#ifndef BATTERY_VOLTAGE_LEVEL_H__
#define BATTERY_VOLTAGE_LEVEL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define ADC_ON_PIN_NUMBER       02
#define ADC_ON_ACTIVE_LEVEL     01


typedef enum {
    
    BATTERY_VOLTAGE_START,
    
//    BATTERY_VOLTAGE_AD_START_WAIT,
//    
//    BATTERY_VOLTAGE_AD_SAMPLE,
//    
//    BATTERY_VOLTAGE_AD_SAMPLE_WAIT,
    
    BATTERY_VOLTAGE_AD_RESTART_WAIT,
    
} battery_voltage_state_t;

typedef struct {
    
//    uint32_t    sample_cnt;
//    uint32_t    sample_N;
    
//    uint32_t    start_wait_timeout_cnt;
//    uint32_t    start_wait_timeout_N;
//    
//    uint32_t    sample_wait_timeout_cnt;
//    uint32_t    sample_wait_timeout_N;
//    
    uint32_t    restart_wait_timeout_cnt;
    uint32_t    restart_wait_timeout_N;
    
} battery_voltage_data_t;

typedef struct {
    
    battery_voltage_state_t     state;
    battery_voltage_data_t      data;
    
} battery_voltage_manage_t;


extern battery_voltage_manage_t     g_battery_voltage_manage;
    
    
int battery_voltage_init(battery_voltage_manage_t *p_battery_voltage_manage);

int battery_voltage_check_state(battery_voltage_manage_t *p_battery_voltage_manage);
    
#ifdef __cplusplus
}
#endif

#endif // BATTERY_VOLTAGE_LEVEL_H__

