/*
 * registers.c
 *
 *  Created on: Apr 13, 2021
 *      Author: jgamm
 */

// This file contains basic functions to communicate with the AT86RF233 as described in the datasheet.

#include "hal.h"
#include "registers.h"
#include "usci_b_spi.h"
#include "gpio.h"
#include "ucs.h"

// This function initializes the MSP430 SPI module and GPIO pins that will be used to talk to the AT86RF233.
void    SPI_init(void)
{
    USCI_B_SPI_initMasterParam settings = // Proper SPI settings based on information in the datasheet.
    {
     .selectClockSource = USCI_B_SPI_CLOCKSOURCE_SMCLK,
     .clockSourceFrequency = UCS_getSMCLK(),
     .desiredSpiClock = AT86_SPI_FREQ,
     .msbFirst = USCI_B_SPI_MSB_FIRST,
     .clockPhase = USCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
     .clockPolarity = USCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW
    };
    USCI_B_SPI_initMaster(AT86_SPI_BASE, &settings); // Initialize the SPI module.
    USCI_B_SPI_enable(AT86_SPI_BASE); // Enable the SPI module.

    GPIO_setAsOutputPin(AT86_SS_PORT, AT86_SS_PIN); // Initialize the SS GPIO pin.
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN);
    GPIO_setAsPeripheralModuleFunctionInputPin(AT86_MOSI_PORT, AT86_MOSI_PIN); // Initialize the MOSI GPIO pin.
    GPIO_setAsPeripheralModuleFunctionInputPin(AT86_SCK_PORT, AT86_SCK_PIN); // Initialize the SPI clock GPIO pin.
    GPIO_setAsPeripheralModuleFunctionInputPin(AT86_MISO_PORT, AT86_MISO_PIN); // Initialize the MISO GPIO pin.
}

// This function transmits a byte to the AT86RF233 over SPI.
//  value: byte to be transmitted.
static void _tx(uint8_t value)
{
    while(!USCI_B_SPI_getInterruptStatus(AT86_SPI_BASE, USCI_B_SPI_TRANSMIT_INTERRUPT)); // Wait until the SPI module is not transmitting.
    USCI_B_SPI_transmitData(AT86_SPI_BASE, value); // Transmit a byte over SPI.
    while(!USCI_B_SPI_getInterruptStatus(AT86_SPI_BASE, USCI_B_SPI_RECEIVE_INTERRUPT)); // Ensure we get a byte in return.
    USCI_B_SPI_receiveData(AT86_SPI_BASE); // Retrieve but do not record the byte, as it does not contain useful data.
}

// This function receives a byte from the AT86RF233 over SPI, and returns the byte.
static uint8_t _rx(void)
{
    while(!USCI_B_SPI_getInterruptStatus(AT86_SPI_BASE, USCI_B_SPI_TRANSMIT_INTERRUPT)); // Wait until the SPI module is not transmitting.
    USCI_B_SPI_transmitData(AT86_SPI_BASE, 0x00); // Transmit a byte (doesn't matter what the byte is).
    while(!USCI_B_SPI_getInterruptStatus(AT86_SPI_BASE, USCI_B_SPI_RECEIVE_INTERRUPT)); // Ensure we receive a byte in return.
    return USCI_B_SPI_receiveData(AT86_SPI_BASE); // Retrieve and return the byte we get.
}

// This function sets the value of an AT86RF233 register, as described in the datasheet.
//  address: Address of the register we want to write.
//  value: Value to set the register to.
void    REG_write(uint8_t address, uint8_t value)
{
    __disable_interrupt(); // An interrupt here could cause us to try to read the status register in the middle of another read operation -- problematic.
    GPIO_setOutputLowOnPin(AT86_SS_PORT, AT86_SS_PIN); // Activate slave select pin
    _tx(address|0xC0); // Transmit address with 2 MSBs active to denote we want to write to this register
    _tx(value); // Transmit the value to set the register to.
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN); // Unselect the AT86RF233 as an SPI slave.
    __enable_interrupt(); // Turn interrupts back on.
}

// This funciton reads the value of an AT86RF233 register, as described in the datasheet.
//  address: Address of the register we want to read.
uint8_t REG_read(uint8_t address)
{
    __disable_interrupt(); // Make sure we don't have other SPI operations until this operation finishes.
    GPIO_setOutputLowOnPin(AT86_SS_PORT, AT86_SS_PIN); // Select the AT86RF233 as SPI slave.
    _tx(address|0x80); // Transmit address with MSB high to denote we want to read the register.
    uint8_t rv = _rx(); // Receive the register value sent by the AT86RF233.
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN); // Unselect the AT86RF233 as SPI slave.
    __enable_interrupt(); // Interrupts can happen again.
    return rv; // Return register value.
}

// This function reads a subset of the AT86RF233 TRX buffer.
//  offset: byte number at which we want to start reading.
//  dest: address in MSP430 memory at which to store the bytes we read.
//  len: number of bytes to read.
void    SRAM_read(uint8_t offset, uint8_t * dest, uint8_t len)
{
    __disable_interrupt(); // Make sure we don't have other SPI operations until this operation finishes.
    GPIO_setOutputLowOnPin(AT86_SS_PORT, AT86_SS_PIN); // Select the AT86RF233 as SPI slave.
    _tx(0x00); // Transmit 0 to indicate we want to do an SRAM read.
    _tx(offset); // Transmit the offset value.
    uint8_t idx; // Receive the desired number of bytes.
    for(idx=0; idx<len; ++idx)
        dest[idx] = _rx();
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN); // Unselect the AT86RF233 as SPI slave.
    __enable_interrupt();  // Interrupts can happen again.
}

// This function writes data to a subset of the AT86RF233 TRX buffer.
//  offset: byte number at which we want to start writing.
//  src: address in MSP430 memory from which to take data.
//  len: number of bytes we want to write.
void    SRAM_write(uint8_t offset, const uint8_t * src, uint8_t len)
{
    __disable_interrupt(); // Make sure we don't have other SPI operations until this operation finishes.
    GPIO_setOutputLowOnPin(AT86_SS_PORT, AT86_SS_PIN); // Select the AT86RF233 as SPI slave.
    _tx(0x40); // Inticate that we want to do an SRAM write.
    _tx(offset); // Transmit the offset.
    uint8_t idx; // Transmit the desired number of bytes.
    for(idx=0; idx<len; ++idx)
        _tx(src[idx]);
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN); // Unselect the AT86RF233 as SPI slave.
    __enable_interrupt(); // Interrupts can happen again.
}

// This function reads from the beginning of the AT86RF233 TRX buffer, as described in the datasheet. It is redundant with the SRAM_read functionality, but is a separate functionality implemented in the AT86RF233.
//  dest: address in MSP430 memory at which retrieved data should be stored.
//  len: number of bytes we want to retrieve.
void    FB_read(uint8_t * dest, uint8_t len)
{
    __disable_interrupt(); // Make sure we don't have other SPI operations until this operation finishes.
    GPIO_setOutputLowOnPin(AT86_SS_PORT, AT86_SS_PIN); // Select the AT86RF233 as SPI slave.
    _tx(0x20); // Indicate that we want to do an FB_read operation.
    uint8_t idx; // Read the desired number of bytes.
    for(idx=0; idx<len; ++idx)
        dest[idx] = _rx();
    GPIO_setOutputHighOnPin(AT86_SS_PORT, AT86_SS_PIN); // Unselect the AT86RF233 as SPI slave.
    __enable_interrupt(); // Interrupts can happen again.
}
