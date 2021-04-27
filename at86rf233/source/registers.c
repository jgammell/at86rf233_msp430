/*
 * registers.c
 *
 *  Created on: Apr 13, 2021
 *      Author: jgamm
 */

#include "hal.h"
#include "registers.h"
#include "usci_b_spi.h"
#include "gpio.h"
#include "ucs.h"

void    SPI_init(void)
{
    USCI_B_SPI_initMasterParam settings =
    {
     .selectClockSource = USCI_B_SPI_CLOCKSOURCE_SMCLK,
     .clockSourceFrequency = UCS_getSMCLK(),
     .desiredSpiClock = AT86_SPI_FREQ,
     .msbFirst = USCI_B_SPI_MSB_FIRST,
     .clockPhase = USCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
     .clockPolarity = USCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW
    };
    USCI_B_SPI_initMaster(AT86_SPI_BASE, &settings);
    USCI_B_SPI_enable(AT86_SPI_BASE);

    GPIO_setAsOutputPin(AT86_SS_PORT, AT86_SS_PIN);
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN);
    GPIO_setAsPeripheralModuleFunctionInputPin(AT86_MOSI_PORT, AT86_MOSI_PIN);
    GPIO_setAsPeripheralModuleFunctionInputPin(AT86_SCK_PORT, AT86_SCK_PIN);
    GPIO_setAsPeripheralModuleFunctionInputPin(AT86_MISO_PORT, AT86_MISO_PIN);
}

static void _tx(uint8_t value)
{
    while(!USCI_B_SPI_getInterruptStatus(AT86_SPI_BASE, USCI_B_SPI_TRANSMIT_INTERRUPT));
    USCI_B_SPI_transmitData(AT86_SPI_BASE, value);
    while(!USCI_B_SPI_getInterruptStatus(AT86_SPI_BASE, USCI_B_SPI_RECEIVE_INTERRUPT));
    USCI_B_SPI_receiveData(AT86_SPI_BASE);
}

static uint8_t _rx(void)
{
    while(!USCI_B_SPI_getInterruptStatus(AT86_SPI_BASE, USCI_B_SPI_TRANSMIT_INTERRUPT));
    USCI_B_SPI_transmitData(AT86_SPI_BASE, 0x00);
    while(!USCI_B_SPI_getInterruptStatus(AT86_SPI_BASE, USCI_B_SPI_RECEIVE_INTERRUPT));
    return USCI_B_SPI_receiveData(AT86_SPI_BASE);
}

void    REG_write(uint8_t address, uint8_t value)
{
    GPIO_setOutputLowOnPin(AT86_SS_PORT, AT86_SS_PIN);
    _tx(address|0xC0);
    _tx(value);
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN);
}

uint8_t REG_read(uint8_t address)
{
    GPIO_setOutputLowOnPin(AT86_SS_PORT, AT86_SS_PIN);
    _tx(address|0x80);
    uint8_t rv = _rx();
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN);
    return rv;
}

void    SRAM_read(uint8_t offset, uint8_t * dest, uint8_t len)
{
    GPIO_setOutputLowOnPin(AT86_SS_PORT, AT86_SS_PIN);
    _tx(0x00);
    _tx(offset);
    uint8_t idx;
    for(idx=0; idx<len; ++idx)
        dest[idx] = _rx();
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN);
}

void    SRAM_write(uint8_t offset, const uint8_t * src, uint8_t len)
{
    GPIO_setOutputLowOnPin(AT86_SS_PORT, AT86_SS_PIN);
    _tx(0x40);
    _tx(offset);
    uint8_t idx;
    for(idx=0; idx<len; ++idx)
        _tx(src[idx]);
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN);
}

void    FB_read(uint8_t * dest, uint8_t len)
{
    GPIO_setOutputLowOnPin(AT86_SS_PORT, AT86_SS_PIN);
    _tx(0x20);
    uint8_t idx;
    for(idx=0; idx<len; ++idx)
        dest[idx] = _rx();
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN);
}
