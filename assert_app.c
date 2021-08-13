/*
 * assert_app.c
 *
 *  Created on: Apr 27, 2021
 *      Author: jgamm
 */

// This file defines an assert statement which will allow us to abort execution of the firmware if errors happen.

#include "assert_app.h"
#include "gpio.h"
#include "hal.h"

// This function allows us to abort firmware if errors happen. If the condition evaluates to false, the code will enter an infinite loop while flashing LEDs 1 and 2.
void assert(bool condition)
{
    if(!condition) // Condition evaluated to false
    {
        GPIO_setAsOutputPin(MCU_LED1_PORT, MCU_LED1_PIN); // Configure LED GPIO pins
        GPIO_setAsOutputPin(MCU_LED2_PORT, MCU_LED2_PIN);
        GPIO_setOutputLowOnPin(MCU_LED1_PORT, MCU_LED1_PIN);
        GPIO_setOutputHighOnPin(MCU_LED2_PORT, MCU_LED2_PIN);
        uint32_t delay_idx;
        while(1) // Infinite loop
        {
            for(delay_idx=100000; delay_idx>0; --delay_idx);
            GPIO_toggleOutputOnPin(MCU_LED1_PORT, MCU_LED1_PIN); // Toggle LEDs
            GPIO_toggleOutputOnPin(MCU_LED2_PORT, MCU_LED2_PIN);
        }
    } // Else, do nothing
}
