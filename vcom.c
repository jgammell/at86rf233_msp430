/*
 * vcom.c
 *
 *  Created on: Apr 27, 2021
 *      Author: jgamm
 */

// This file contains functions that will allow us to talk to the computer over USB. The MSP-EXP430F5529LP has a USB chip which we can talk to
// using a UART; the chip will relay everything we say to the computer using a USB virtual COM port.


#include "vcom.h" // Declarations of functions/macros in this file
#include "hal.h" // Definitions of pins, peripheral initialization details, etc.
#include "gpio.h" // TI-provided file to control MSP430 GPIO pins
#include "usci_a_uart.h" // TI-provided file to control MSP430 UART
#include "assert_app.h" // Assert statements so we can abort code if errors happen

static volatile uint8_t uart_rx[256]; // Buffer to store strings we receive over VCOM
static volatile uint8_t uart_rx_idx = 0; // Index of above buffer
static volatile bool uart_rx_available = false; // Track whether buffer contains a complete command
#define UART_BREAKCHAR ('\n') // Character indicating end of a complete command

static volatile uint8_t uart_tx[256]; // Buffer to store strings we are going to transmit over VCOM
static volatile uint8_t uart_tx_len = 0; // Total length of string we will send
static volatile uint8_t uart_tx_idx = 0; // Index of above buffer

// Interrupt triggered when hardware UART requires our attention
#pragma vector=USCI_A1_VECTOR
void __attribute__ ((interrupt)) serviceUart(void)
{
    uint8_t flags = USCI_A_UART_getInterruptStatus(MCU_BCUA_UCA, HWREG8(MCU_BCUA_UCA+OFS_UCAxIE)); // Figure out which interrupts happened
    USCI_A_UART_clearInterrupt(MCU_BCUA_UCA, 0xFF); // Clear interrupt flags so we can detect future interrupts
    if(flags & USCI_A_UART_RECEIVE_INTERRUPT_FLAG) // UART has received a character
    {
        char rv = USCI_A_UART_receiveData(MCU_BCUA_UCA); // Read the character
        if(rv == UART_BREAKCHAR) // Character indicates completion of a command
            uart_rx_available = true; // Make note of this
        else
            uart_rx[uart_rx_idx] = rv; // Store the character in the RX buffer
        ++uart_rx_idx;
    }

    if(flags & USCI_A_UART_TRANSMIT_INTERRUPT_FLAG) // UART has finished transmitting a character
    {
        if(uart_tx_idx < uart_tx_len) // There are more characters to transmit
        {
            USCI_A_UART_transmitData(MCU_BCUA_UCA, uart_tx[uart_tx_idx]); // Load the next character into the UART
            ++uart_tx_idx;
        }
        else // We are done transmitting characters
        {
            uart_tx_idx = 0; // Reset buffer indices
            uart_tx_len = 0;
            USCI_A_UART_disableInterrupt(MCU_BCUA_UCA, USCI_A_UART_TRANSMIT_INTERRUPT); // Stop sending interrupts after characters are transmitted
        }
    }
}

// This function uses the UART to transmit a string to a USB chip, which will send it to the computer using a virtual COM port.
//  data: base address of string we want to transmit
//  length: number of characters to transmit
void VCOM_tx(uint8_t * data, uint8_t len)
{
    assert(uart_tx_len == 0); // Ensure we are not in the middle of transmitting a string
    uart_tx_len = len; // Record length of string to transmit
    uart_tx_idx = 0; // Ensure buffer index is reset
    uint8_t idx;
    for(idx=0; idx<len; ++idx) // Store string in the transmit buffer
        uart_tx[idx] = data[idx];
    USCI_A_UART_enableInterrupt(MCU_BCUA_UCA, USCI_A_UART_TRANSMIT_INTERRUPT); // UART will interrupt every time its TX buffer is empty and we can transmit a new character
    USCI_A_UART_transmitData(MCU_BCUA_UCA, uart_tx[0]); // Transmit the first character (subsequent characters will be transmitted in interrupt handler)
    ++uart_tx_idx;
}

// This function initializes the UART so we can use it to talk to the virtual COM port.
void VCOM_init(void)
{
    GPIO_setAsPeripheralModuleFunctionInputPin(MCU_BCUARX_PORT, MCU_BCUARX_PIN); // Initialize RX GPIO pin
    GPIO_setAsPeripheralModuleFunctionOutputPin(MCU_BCUATX_PORT, MCU_BCUATX_PIN); // Initialize TX GPIO pin
    USCI_A_UART_initParam uart_settings = // Initialize the UART we will be using
    {
     .selectClockSource = USCI_A_UART_CLOCKSOURCE_SMCLK,
     .clockPrescalar = 10,
     .firstModReg = 14,
     .secondModReg = 0,
     .parity = USCI_A_UART_NO_PARITY,
     .msborLsbFirst = USCI_A_UART_LSB_FIRST,
     .numberofStopBits = USCI_A_UART_ONE_STOP_BIT,
     .uartMode = USCI_A_UART_MODE,
     .overSampling = USCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
    };
    USCI_A_UART_init(MCU_BCUA_UCA, &uart_settings);
    USCI_A_UART_enable(MCU_BCUA_UCA); // Enable the UART
    USCI_A_UART_enableInterrupt(MCU_BCUA_UCA, USCI_A_UART_RECEIVE_INTERRUPT); // UART will interrupt every time we receive a character
}

// This function indicates whether the RX buffer contains a complete command (true = yes).
bool VCOM_rxAvailable(void)
{
    return uart_rx_available;
}

// This function returns the base address of the command stored in the RX buffer.
char * VCOM_getRxString(void)
{
    uart_rx[uart_rx_idx-1] = '\0'; // Add null character to end so it can be parsed using standard C string libraries
    uart_rx_idx = 0; // Indicate RX buffer no longer contains valid string
    uart_rx_available = false;
    return (char *) uart_rx; // Return base address
}

// This function indicates whether we are currently transmitting a string over the VCOM port (true = yes).
bool VCOM_isTransmitting(void)
{
    return uart_tx_len!=0;
}


