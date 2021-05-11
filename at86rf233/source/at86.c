/*
 * at86.c
 *
 *  Created on: Apr 13, 2021
 *      Author: jgamm
 */

#include "at86.h"
#include "registers.h"
#include "gpio.h"
#include "hal.h"

volatile bool irq_pending = false;

void AT86_init(void)
{
    GPIO_setAsOutputPin(AT86_PWR_PORT, AT86_PWR_PIN);
    GPIO_setDriveStrength(AT86_PWR_PORT, AT86_PWR_PIN, GPIO_FULL_OUTPUT_DRIVE_STRENGTH);
    GPIO_setAsOutputPin(AT86_RESET_PORT, AT86_RESET_PIN);
    GPIO_setOutputHighOnPin(AT86_RESET_PORT, AT86_RESET_PIN);
    GPIO_setAsOutputPin(AT86_WAKEUP_PORT, AT86_WAKEUP_PIN);
    GPIO_setOutputLowOnPin(AT86_WAKEUP_PORT, AT86_WAKEUP_PIN);
    REG_write(REG__IRQ_MASK, 0);
    REG_read(REG__IRQ_STATUS);
    GPIO_setAsInputPin(AT86_IRQ_PORT, AT86_IRQ_PIN);
    GPIO_disableInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
    GPIO_selectInterruptEdge(AT86_IRQ_PORT, AT86_IRQ_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
    SPI_init();
    GPIO_setOutputHighOnPin(AT86_PWR_PORT, AT86_PWR_PIN);
    volatile uint32_t delay_idx;
    for(delay_idx=100000; delay_idx>0; --delay_idx);
    AT86_sendCmd(cmdFORCE_TRX_OFF);
    AT86_Status_Enum status = AT86_getStatus();
    while(status != statusTRX_OFF)
        status = AT86_getStatus();
}

uint8_t AT86_getPartNum(void)
{
    uint8_t tmp = REG_read(REG__PART_NUM);
    tmp &= MASK__PART_NUM__PART_NUM;
    tmp >>= SHIFT__PART_NUM__PART_NUM;
    return tmp;
}

uint8_t AT86_getVersionNum(void)
{
    uint8_t tmp = REG_read(REG__VERSION_NUM);
    tmp &= MASK__VERSION_NUM__VERSION_NUM;
    tmp >>= SHIFT__VERSION_NUM__VERSION_NUM;
    return tmp;
}

uint16_t AT86_getManId(void)
{
    uint16_t tmp0 = REG_read(REG__MAN_ID_0);
    tmp0 &= MASK__MAN_ID_0__MAN_ID_0;
    tmp0 >>= SHIFT__MAN_ID_0__MAN_ID_0;
    uint16_t tmp1 = REG_read(REG__MAN_ID_1);
    tmp1 &= MASK__MAN_ID_1__MAN_ID_1;
    tmp1 >>= SHIFT__MAN_ID_1__MAN_ID_1;
    uint16_t rv = (tmp1<<8) | tmp0;
    return rv;
}

uint16_t AT86_getAddrShort(void)
{
    uint16_t tmp0 = REG_read(REG__SHORT_ADDR_0);
    tmp0 &= MASK__SHORT_ADDR_0__SHORT_ADDR_0;
    tmp0 >>= SHIFT__SHORT_ADDR_0__SHORT_ADDR_0;
    uint16_t tmp1 = REG_read(REG__SHORT_ADDR_1);
    tmp1 &= MASK__SHORT_ADDR_1__SHORT_ADDR_1;
    tmp1 >>= SHIFT__SHORT_ADDR_1__SHORT_ADDR_1;
    uint16_t rv = (tmp1<<8) | tmp0;
    return rv;
}

void AT86_setAddrShort(uint16_t address)
{
    REG_write(REG__SHORT_ADDR_0, address&0xFF);
    REG_write(REG__SHORT_ADDR_1, address>>8);
}

uint8_t AT86_getChan(void)
{
    uint8_t rv = REG_read(REG__PHY_CC_CCA);
    rv &= MASK__PHY_CC_CCA__CHANNEL;
    rv >>= SHIFT__PHY_CC_CCA__CHANNEL;
    return rv;
}

void AT86_setChan(uint8_t channel)
{
    uint8_t tmp = REG_read(REG__PHY_CC_CCA);
    tmp &= ~MASK__PHY_CC_CCA__CHANNEL;
    tmp |= (channel<<SHIFT__PHY_CC_CCA__CHANNEL);
    REG_write(REG__PHY_CC_CCA, tmp);
}

uint16_t AT86_getPan(void)
{
    return (((uint16_t) REG_read(REG__PAN_ID_1))<<8) | ((uint16_t) REG_read(REG__PAN_ID_0));
}

void AT86_setPan(uint16_t pan)
{
    REG_write(REG__PAN_ID_0, pan&0xFF);
    REG_write(REG__PAN_ID_1, pan>>8);
}

int16_t AT86_getTxPower(void)
{
    static const int16_t tx_pow_to_dbm[] =
    {4, 4, 3, 3, 2, 2, 1, 0, -1, -2, -3, -4, -6, -8, -12, -17};
    uint8_t tmp = REG_read(REG__PHY_TX_PWR);
    tmp &= MASK__PHY_TX_PWR__TX_PWR;
    tmp >>= SHIFT__PHY_TX_PWR__TX_PWR;
    return tx_pow_to_dbm[tmp];
}

void AT86_setTxPower(int16_t power)
{
    static const uint8_t dbm_to_tx_pow[] =
    {0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
     0x0e, 0x0d, 0x0d, 0x0d, 0x0c, 0x0c,
     0x0b, 0x0b, 0x0a, 0x09, 0x08, 0x07,
     0x06, 0x05, 0x03,0x00};
    power += 17;
    if(power<0)
        power = 0;
    else if(power>21)
        power = 21;
    REG_write(REG__PHY_TX_PWR, dbm_to_tx_pow[power]);
}

AT86_Status_Enum AT86_getStatus(void)
{
    uint8_t tmp = REG_read(REG__TRX_STATUS);
    tmp &= MASK__TRX_STATUS__TRX_STATUS;
    tmp >>= SHIFT__TRX_STATUS__TRX_STATUS;
    return (AT86_Status_Enum) tmp;
}

void AT86_sendCmd(AT86_Cmd_Enum cmd)
{
    REG_write(REG__TRX_STATE, cmd);
}

void AT86_enablePhase(bool enable)
{
    uint8_t temp = REG_read(REG__TRX_CTRL_0);
    temp &= ~(MASK__TRX_CTRL_0__PMU_EN);
    temp |= enable<<SHIFT__TRX_CTRL_0__PMU_EN;
    REG_write(REG__TRX_CTRL_0, temp);
}

uint8_t AT86_getPhase(void)
{
    return REG_read(REG__PHY_PMU_VALUE);
}

void AT86_prepareTx(void)
{
    AT86_Status_Enum status;
    do
    {
        status = AT86_getStatus();
    } while(status == statusBUSY_TX);

    if(status == statusBUSY_RX)
        AT86_sendCmd(cmdFORCE_TRX_OFF);
    AT86_sendCmd(cmdPLL_ON);
}

void AT86_loadTx(const uint8_t * src, uint8_t len, uint8_t offset)
{
    SRAM_write(0, &len, 1);
    SRAM_write(offset+1, src, len);
}

void AT86_execTx(void)
{
    AT86_sendCmd(cmdTX_START);
}

void AT86_prepareRx(void)
{
    REG_write(REG__IRQ_MASK, irqRX_START);
    GPIO_clearInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
    GPIO_enableInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
    AT86_sendCmd(cmdRX_ON);
}

void AT86_execRx(void)
{
    REG_write(REG__IRQ_MASK, irqTRX_END);
    GPIO_clearInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
    GPIO_enableInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
}

void AT86_readRx(uint8_t * dest, uint8_t len, uint8_t offset)
{
    REG_write(REG__IRQ_MASK, 0);
    SRAM_read(offset, dest, len);
}

AT86_Irq_Enum AT86_readIstat(void)
{
    irq_pending = false;
    return (AT86_Irq_Enum) REG_read(REG__IRQ_STATUS);
}

bool AT86_irqPending(void)
{
    return irq_pending;
}

void __attribute__ ((interrupt)) _pinIrqHandler(void)
{
    irq_pending = true;
    GPIO_disableInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
}
