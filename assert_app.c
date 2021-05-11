/*
 * assert_app.c
 *
 *  Created on: Apr 27, 2021
 *      Author: jgamm
 */

#include "assert_app.h"
#include "gpio.h"
#include "hal.h"


void assert(bool condition)
{
    if(!condition)
    {
        GPIO_setAsOutputPin(MCU_LED1_PORT, MCU_LED1_PIN);
        GPIO_setAsOutputPin(MCU_LED2_PORT, MCU_LED2_PIN);
        GPIO_setOutputLowOnPin(MCU_LED1_PORT, MCU_LED1_PIN);
        GPIO_setOutputHighOnPin(MCU_LED2_PORT, MCU_LED2_PIN);
        while(1)
        {
            volatile uint32_t delay_idx;
            for(delay_idx=100000; delay_idx>0; --delay_idx);
            GPIO_toggleOutputOnPin(MCU_LED1_PORT, MCU_LED1_PIN);
            GPIO_toggleOutputOnPin(MCU_LED2_PORT, MCU_LED2_PIN);
        }
    }
}
