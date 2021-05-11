/*
 * vcom.c
 *
 *  Created on: Apr 27, 2021
 *      Author: jgamm
 */


#include "vcom.h"
#include "hal.h"
#include "gpio.h"
#include "usci_a_uart.h"
#include "assert_app.h"

static volatile uint8_t uart_rx[256];
static volatile uint8_t uart_rx_idx = 0;
static volatile bool uart_rx_available = false;
#define UART_BREAKCHAR ('\n')

static uint8_t uart_tx[256];
static uint8_t uart_tx_len = 0;
static volatile uint8_t uart_tx_idx = 0;


#pragma vector=USCI_A1_VECTOR
void __attribute__ ((interrupt)) serviceUart(void)
{
    uint8_t flags = USCI_A_UART_getInterruptStatus(MCU_BCUA_UCA, HWREG8(MCU_BCUA_UCA+OFS_UCAxIE));
    USCI_A_UART_clearInterrupt(MCU_BCUA_UCA, 0xFF);
    if(flags & USCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        char rv = USCI_A_UART_receiveData(MCU_BCUA_UCA);
        if(rv == UART_BREAKCHAR)
            uart_rx_available = true;
        else
            uart_rx[uart_rx_idx] = rv;
        ++uart_rx_idx;
    }

    if(flags & USCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
    {
        if(uart_tx_idx < uart_tx_len)
        {
            USCI_A_UART_transmitData(MCU_BCUA_UCA, uart_tx[uart_tx_idx]);
            ++uart_tx_idx;
        }
        else
        {
            uart_tx_idx = 0;
            uart_tx_len = 0;
            USCI_A_UART_disableInterrupt(MCU_BCUA_UCA, USCI_A_UART_TRANSMIT_INTERRUPT);
        }
    }
}


void VCOM_tx(uint8_t * data, uint8_t len)
{
    assert(uart_tx_len == 0);
    uart_tx_len = len;
    uart_tx_idx = 0;
    uint8_t idx;
    for(idx=0; idx<len; ++idx)
        uart_tx[idx] = data[idx];
    USCI_A_UART_enableInterrupt(MCU_BCUA_UCA, USCI_A_UART_TRANSMIT_INTERRUPT);
    USCI_A_UART_transmitData(MCU_BCUA_UCA, uart_tx[0]);
    ++uart_tx_idx;
}

void VCOM_init(void)
{
    GPIO_setAsPeripheralModuleFunctionInputPin(MCU_BCUARX_PORT, MCU_BCUARX_PIN);
    GPIO_setAsPeripheralModuleFunctionOutputPin(MCU_BCUATX_PORT, MCU_BCUATX_PIN);
    USCI_A_UART_initParam uart_settings =
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
    USCI_A_UART_enable(MCU_BCUA_UCA);
    USCI_A_UART_enableInterrupt(MCU_BCUA_UCA, USCI_A_UART_RECEIVE_INTERRUPT);
}

bool VCOM_rxAvailable(void)
{
    return uart_rx_available;
}

char * VCOM_getRxString(void)
{
    uart_rx[uart_rx_idx-1] = '\0';
    uart_rx_idx = 0;
    uart_rx_available = false;
    return (char *) uart_rx;
}

bool VCOM_isTransmitting(void)
{
    return uart_tx_len!=0;
}


