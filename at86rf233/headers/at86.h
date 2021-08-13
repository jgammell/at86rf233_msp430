/*
 * at86.h
 *
 *  Created on: Apr 13, 2021
 *      Author: jgamm
 */

// This file contains declarations of functions defined in at86.c (see this file for details). These functions facilitate communication with the AT86RF233.

#ifndef AT86RF233_HEADERS_AT86_H_
#define AT86RF233_HEADERS_AT86_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum // List of states the AT86RF233 may be in. See section 7 of the datasheet for details.
{
    statusP_ON                         = 0x00,
    statusBUSY_RX                      = 0x01,
    statusBUSY_TX                      = 0x02,
    statusRX_ON                        = 0x06,
    statusTRX_OFF                      = 0x08,
    statusPLL_ON                       = 0x09,
    statusSLEEP                        = 0x0F,
    statusPREP_DEEP_SLEEP              = 0x10,
    statusBUSY_RX_AACK                 = 0x11,
    statusBUSY_TX_ARET                 = 0x12,
    statusRX_AACK_ON                   = 0x16,
    statusTX_ARET_ON                   = 0x19,
    statusSTATE_TRANSITION_IN_PROGRESS = 0x1F
} AT86_Status_Enum;

typedef enum // List of commands that can be sent to the AT86RF233. See section 7 of the datasheet for details.
{
    cmdNOP             = 0x00,
    cmdTX_START        = 0x02,
    cmdFORCE_TRX_OFF   = 0x03,
    cmdFORCE_PLL_ON    = 0x04,
    cmdRX_ON           = 0x06,
    cmdTRX_OFF         = 0x08,
    cmdPLL_ON          = 0x09,
    cmdPREP_DEEP_SLEEP = 0x10,
    cmdRX_AACK_ON      = 0x16,
    cmdTX_ARET_ON      = 0x19
} AT86_Cmd_Enum;

typedef enum // List of interrupts that may be issued by the AT86RF233. See section 7 of the datasheet for details.
{
    irqPLL_LOCK = 0x01,
    irqPLL_UNLOCK = 0x02,
    irqRX_START = 0x04,
    irqTRX_END = 0x08,
    irqCCA_ED_DONE = 0x10,
    irqAMI = 0x20,
    irqTRX_UR = 0x40,
    irqBAT_LOW = 0x80
} AT86_Irq_Enum;

void AT86_init(void); // Initialize the MSP430 peripherals used to control the AT86RF233, and put the AT86RF233 in an idle state.

uint8_t AT86_getPartNum(void); // Retrieve the part number of the AT86RF233.

uint8_t AT86_getVersionNum(void); // Retrieve the version number of the AT86RF233.

uint16_t AT86_getManId(void); // Retrieve the manufacturing ID of the AT86RF233.

uint16_t AT86_getAddrShort(void); // Retrieve the short address of the AT86RF233.

void AT86_setAddrShort(uint16_t address); // Configure the short address of the AT86RF233.

uint8_t AT86_getChan(void); // Get the current channel on which the AT86RF233 is transmitting and receiving.

void AT86_setChan(uint8_t chan); // Configure the channel on which the AT86RF233 will transmit and receive.

uint16_t AT86_getPan(void); // Get the PAN ID of the AT86RF233.

void AT86_setPan(uint16_t pan); // Configure the PAN ID of the AT86RF233.

int16_t AT86_getTxPower(void); // Get the power (dBm) at which the AT86RF233 will transmit.

void AT86_setTxPower(int16_t power); // Configure the power (dBm) at which the AT86RF233 will transmit.

AT86_Status_Enum AT86_getStatus(void); // Read the status register of the AT86RF233, indicating its current state.

void AT86_sendCmd(AT86_Cmd_Enum cmd); // Send one of a list of commands to the AT86RF233.

void AT86_enablePhase(bool enable); // Enable or disable phase measurement by the AT86RF233.

uint8_t AT86_getPhase(void); // Read the latest phase measurement by the AT86RF233.

void AT86_prepareTx(void); // Put the AT86RF233 in a state from which it can transmit when prompted.

void AT86_loadTx(const uint8_t * src, uint8_t len, uint8_t offset); // Configure the next payload to be transmitted.

void AT86_execTx(void); // Transmit the payload that has been loaded into the buffer of the AT86RF233.

void AT86_prepareRx(void); // Put the AT86RF233 in reception mode.

void AT86_execRx(void); // Acknowledge interrupt indicating that the AT86RF233 has finished receiving a payload.

void AT86_readRx(uint8_t * dest, uint8_t len, uint8_t offset); // Retrieve the latest payload received by the AT86RF233.

AT86_Irq_Enum AT86_readIstat(void); // Read the interrupt status register to determine which AT86RF233 interrupt has happened.

bool AT86_irqPending(void); // Determine whether the AT86RF233 has issued an interrupt that has not yet been dealt with.

#endif /* AT86RF233_HEADERS_AT86_H_ */
