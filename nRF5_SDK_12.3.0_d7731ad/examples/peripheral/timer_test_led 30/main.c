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
 * @defgroup nrf_dev_timer_example_main main.c
 * @{
 * @ingroup nrf_dev_timer_example
 * @brief Timer Example Application main file.
 *
 * This file contains the source code for a sample application using Timer0.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "bsp.h"
#include "app_error.h"
#include "nrf_drv_clock.h"

#include "nrf_delay.h"

const nrf_drv_timer_t TIMER_LED = NRF_DRV_TIMER_INSTANCE(0);

/**
 * @brief Handler for timer events.
 */
void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    static uint32_t i;
    uint32_t led_to_invert = ((i++) % LEDS_NUMBER);

    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
        //    bsp_board_led_invert(led_to_invert);
            
            nrf_gpio_pin_toggle(22);
            nrf_gpio_pin_toggle(0);
            nrf_gpio_pin_toggle(30);
            nrf_gpio_pin_toggle(21);
            nrf_gpio_pin_toggle(28);
            nrf_gpio_pin_toggle(29);
    
            break;

        default:
            //Do nothing.
            break;
    }
}


/**
 * @brief Function for main application entry.
 */
int main(void)
{
    uint32_t time_ms = 1000; //Time(in miliseconds) between consecutive compare events.
    uint32_t time_ticks;
    uint32_t err_code = NRF_SUCCESS;
  
  int n = 3000;
  (void)err_code;
  
      err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    //Configure all leds on board.
    //bsp_board_leds_init();
    /**/
    //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
//    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
//    err_code = nrf_drv_timer_init(&TIMER_LED, &timer_cfg, timer_led_event_handler);
//    APP_ERROR_CHECK(err_code);

//    time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_LED, time_ms);

////    nrf_drv_timer_extended_compare(
////         &TIMER_LED, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

//    nrf_drv_timer_enable(&TIMER_LED);

    nrf_gpio_cfg_output(22);
    nrf_gpio_cfg_output(0);
    
    nrf_gpio_cfg_output(30);
    nrf_gpio_cfg_output(21);
    nrf_gpio_cfg_output(28);
    nrf_gpio_cfg_output(29);
    
    
    nrf_gpio_pin_write(22,  0);
    nrf_gpio_pin_write(0,  0);
    nrf_gpio_pin_write(30,  0);
    nrf_gpio_pin_write(21,  0);
    nrf_gpio_pin_write(28,  1);
    nrf_gpio_pin_write(29,  0);
    
    
//    
//    nrf_gpio_pin_toggle(22);
//    nrf_gpio_pin_toggle(0);
//    nrf_gpio_pin_toggle(30);
//    nrf_gpio_pin_toggle(21);
//    nrf_gpio_pin_toggle(28);
//    nrf_gpio_pin_toggle(29);
//    
    nrf_drv_clock_hfclk_request(NULL);
    while (!nrf_drv_clock_hfclk_is_running());
    while (1)
    {
        nrf_delay_ms(999);
//      n = 750000;
//      while(n--);
//              nrf_delay_ms(1000);
//        nrf_drv_clock_lfclk_request(NULL);

//        nrf_delay_ms(1000);
//        nrf_drv_clock_lfclk_release();
      
        
//        nrf_gpio_pin_toggle(22);
//        nrf_gpio_pin_toggle(0);
//        nrf_gpio_pin_toggle(30);
//        nrf_gpio_pin_toggle(21);
//        nrf_gpio_pin_toggle(28);
//        nrf_gpio_pin_toggle(29);
      
    nrf_gpio_pin_write(22,  1);
    nrf_gpio_pin_write(0,  1);
    nrf_gpio_pin_write(30,  1);
    nrf_gpio_pin_write(21,  1);
    nrf_gpio_pin_write(28,  1);
    nrf_gpio_pin_write(29,  1);
      
        nrf_delay_ms(1);
    nrf_gpio_pin_write(22,  0);
    nrf_gpio_pin_write(0,  0);
    nrf_gpio_pin_write(30,  0);
    nrf_gpio_pin_write(21,  0);
    nrf_gpio_pin_write(28,  0);
    nrf_gpio_pin_write(29,  0);
      //  __WFI();
    }
}

/** @} */
