/*
 * at86.h
 *
 *  Created on: Apr 13, 2021
 *      Author: jgamm
 */

#ifndef AT86RF233_HEADERS_AT86_H_
#define AT86RF233_HEADERS_AT86_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
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

typedef enum
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

typedef enum
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

void AT86_init(void);

uint8_t AT86_getPartNum(void);

uint8_t AT86_getVersionNum(void);

uint16_t AT86_getManId(void);

uint16_t AT86_getAddrShort(void);

void AT86_setAddrShort(uint16_t address);

uint8_t AT86_getChan(void);

void AT86_setChan(uint8_t chan);

uint16_t AT86_getPan(void);

void AT86_setPan(uint16_t pan);

int16_t AT86_getTxPower(void);

void AT86_setTxPower(int16_t power);

AT86_Status_Enum AT86_getStatus(void);

void AT86_sendCmd(AT86_Cmd_Enum cmd);

void AT86_enablePhase(bool enable);

uint8_t AT86_getPhase(void);

void AT86_prepareTx(void);

void AT86_loadTx(const uint8_t * src, uint8_t len, uint8_t offset);

void AT86_execTx(void);

void AT86_prepareRx(void);

void AT86_execRx(void);

void AT86_readRx(uint8_t * dest, uint8_t len, uint8_t offset);

AT86_Irq_Enum AT86_readIstat(void);

bool AT86_irqPending(void);

#endif /* AT86RF233_HEADERS_AT86_H_ */
