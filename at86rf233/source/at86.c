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

volatile bool irq_pending = false; // Variable that will be true whenever the AT86RF233 has sent an interrupt that has not yet been addressed by higher-level code.

// This function initializes the MSP430 peripherals used to interact with the AT86RF233, then puts the AT86RF233 in an idle state.
void AT86_init(void)
{
    GPIO_setAsOutputPin(AT86_PWR_PORT, AT86_PWR_PIN); // Initialize GPIO pin used to toggle power to AT86RF233
    GPIO_setDriveStrength(AT86_PWR_PORT, AT86_PWR_PIN, GPIO_FULL_OUTPUT_DRIVE_STRENGTH);
    GPIO_setAsOutputPin(AT86_RESET_PORT, AT86_RESET_PIN); // Initialize GPIO pin going to AT86RF233 RESET pin
    GPIO_setOutputHighOnPin(AT86_RESET_PORT, AT86_RESET_PIN);
    GPIO_setAsOutputPin(AT86_WAKEUP_PORT, AT86_WAKEUP_PIN); // Initialize GPIO pin going to AT86RF233 WAKEUP pin
    GPIO_setOutputLowOnPin(AT86_WAKEUP_PORT, AT86_WAKEUP_PIN);
    GPIO_setAsInputPin(AT86_IRQ_PORT, AT86_IRQ_PIN); // Initialize GPIO pin going to AT86RF233 IRQ pin
    GPIO_disableInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
    GPIO_selectInterruptEdge(AT86_IRQ_PORT, AT86_IRQ_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
    SPI_init(); // Initialize SPI module used to talk to AT86RF233
    GPIO_setOutputHighOnPin(AT86_PWR_PORT, AT86_PWR_PIN); // Supply power to the AT86RF233
    volatile uint32_t delay_idx;
    for(delay_idx=100000; delay_idx>0; --delay_idx); // Delay to give AT86RF233 time to turn on
    REG_write(REG__IRQ_MASK, 0); // Disable interrupts from AT86RF233
    REG_read(REG__IRQ_STATUS); // Clear pending AT86RF233 interrupts
    AT86_sendCmd(cmdFORCE_TRX_OFF); // Put AT86RF233 in idle state
    AT86_Status_Enum status = AT86_getStatus();
    while(status != statusTRX_OFF) // Wait until AT86RF233 has transitioned to idle state
        status = AT86_getStatus();
}

// This function reads and returns the AT86RF233 part number.
uint8_t AT86_getPartNum(void)
{
    uint8_t tmp = REG_read(REG__PART_NUM); // Retrieve value of register containing part number field
    tmp &= MASK__PART_NUM__PART_NUM; // Extract part number field from value
    tmp >>= SHIFT__PART_NUM__PART_NUM;
    return tmp; // Return part number
}

// This function reads and returns the AT86RF233 version number.
uint8_t AT86_getVersionNum(void)
{
    uint8_t tmp = REG_read(REG__VERSION_NUM); // Retrieve value of register containing version number field
    tmp &= MASK__VERSION_NUM__VERSION_NUM; // Extract version number field from value
    tmp >>= SHIFT__VERSION_NUM__VERSION_NUM;
    return tmp; // Return version number
}

// This function reads and returns the AT86RF233 manufacturing ID.
uint16_t AT86_getManId(void)
{
    uint16_t tmp0 = REG_read(REG__MAN_ID_0); // Read register containing LSB of manufacturing ID
    tmp0 &= MASK__MAN_ID_0__MAN_ID_0; // Extract manufacturing ID LSB from value
    tmp0 >>= SHIFT__MAN_ID_0__MAN_ID_0;
    uint16_t tmp1 = REG_read(REG__MAN_ID_1); // Read register containing MSB of manufacturing ID
    tmp1 &= MASK__MAN_ID_1__MAN_ID_1; // Extract manufacturing ID MSB from value
    tmp1 >>= SHIFT__MAN_ID_1__MAN_ID_1;
    uint16_t rv = (tmp1<<8) | tmp0; // Construct manufacturing ID
    return rv; // Return manufacturing ID
}

// This function reads and returns the short AT86RF233 address.
uint16_t AT86_getAddrShort(void)
{
    uint16_t tmp0 = REG_read(REG__SHORT_ADDR_0); // Read register containing LSB of short address
    tmp0 &= MASK__SHORT_ADDR_0__SHORT_ADDR_0; // Extract short address LSB from value
    tmp0 >>= SHIFT__SHORT_ADDR_0__SHORT_ADDR_0;
    uint16_t tmp1 = REG_read(REG__SHORT_ADDR_1); // Read register containing MSB of short address
    tmp1 &= MASK__SHORT_ADDR_1__SHORT_ADDR_1; // Extract short address MSB from value
    tmp1 >>= SHIFT__SHORT_ADDR_1__SHORT_ADDR_1;
    uint16_t rv = (tmp1<<8) | tmp0; // Construct short address
    return rv; // Return short address
}

// This function sets the AT86RF233 short address.
void AT86_setAddrShort(uint16_t address)
{
    REG_write(REG__SHORT_ADDR_0, address&0xFF); // Write LSB of short address to register
    REG_write(REG__SHORT_ADDR_1, address>>8); // Write MSB of short address to register
}

// This function reads the current channel on which the AT86RF233 is receiving and transmitting.
uint8_t AT86_getChan(void)
{
    uint8_t rv = REG_read(REG__PHY_CC_CCA); // Read register containing channel
    rv &= MASK__PHY_CC_CCA__CHANNEL; // Extract channel
    rv >>= SHIFT__PHY_CC_CCA__CHANNEL;
    return rv; // Return channel
}

// This function sets the channel on which the AT86RF233 will receive and transmit.
void AT86_setChan(uint8_t channel)
{
    uint8_t tmp = REG_read(REG__PHY_CC_CCA); // Read register containing channel field
    tmp &= ~MASK__PHY_CC_CCA__CHANNEL; // Modify channel field without changing other fields
    tmp |= (channel<<SHIFT__PHY_CC_CCA__CHANNEL);
    REG_write(REG__PHY_CC_CCA, tmp); // Update register value
}

// This function reads the PAN ID of the AT86RF233.
uint16_t AT86_getPan(void)
{
    return (((uint16_t) REG_read(REG__PAN_ID_1))<<8) | ((uint16_t) REG_read(REG__PAN_ID_0)); // construct and return PAN ID from registers containing its MSB and LSB
}

// This function modifies the PAN ID of the AT86RF233.
void AT86_setPan(uint16_t pan)
{
    REG_write(REG__PAN_ID_0, pan&0xFF); // Modify the LSB of the PAN ID
    REG_write(REG__PAN_ID_1, pan>>8); // Modify the MSB of the PAN ID
}

// This function reads the power (dBm) at which the AT86RF233 is currently transmitting.
int16_t AT86_getTxPower(void)
{
    static const int16_t tx_pow_to_dbm[] = // array that converts from return value to actual value in dBm
    {4, 4, 3, 3, 2, 2, 1, 0, -1, -2, -3, -4, -6, -8, -12, -17};
    uint8_t tmp = REG_read(REG__PHY_TX_PWR); // read register containing TX power value
    tmp &= MASK__PHY_TX_PWR__TX_PWR; // extract TX power from register
    tmp >>= SHIFT__PHY_TX_PWR__TX_PWR;
    return tx_pow_to_dbm[tmp]; // convert to dBm units
}

// This function sets the power (dBm) at which the AT86RF233 should transmit.
void AT86_setTxPower(int16_t power)
{
    static const uint8_t dbm_to_tx_pow[] = // array that converts from dBm to units used in register
    {0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
     0x0e, 0x0d, 0x0d, 0x0d, 0x0c, 0x0c,
     0x0b, 0x0b, 0x0a, 0x09, 0x08, 0x07,
     0x06, 0x05, 0x03,0x00};
    power += 17; // map power in dBm to index of conversion array
    if(power<0)
        power = 0;
    else if(power>21)
        power = 21;
    REG_write(REG__PHY_TX_PWR, dbm_to_tx_pow[power]); // set register to value of array corresponding to input power
}

// This function reads the AT86 status register to determine its current state.
AT86_Status_Enum AT86_getStatus(void)
{
    uint8_t tmp = REG_read(REG__TRX_STATUS); // Read register containing status
    tmp &= MASK__TRX_STATUS__TRX_STATUS; // Extract status from value
    tmp >>= SHIFT__TRX_STATUS__TRX_STATUS;
    return (AT86_Status_Enum) tmp; // Return status
}

// This function sends one of a list of commands to the AT86RF233 to cause it to perform some action -- e.g. transmit a payload, or change state.
void AT86_sendCmd(AT86_Cmd_Enum cmd)
{
    REG_write(REG__TRX_STATE, cmd); // Write command to the appropriate register
}

// This function enables or disables phase measurements on received data by the AT86RF233.
void AT86_enablePhase(bool enable)
{
    uint8_t temp = REG_read(REG__TRX_CTRL_0); // Read register containing enable bit
    temp &= ~(MASK__TRX_CTRL_0__PMU_EN); // Modify enable bit
    temp |= enable<<SHIFT__TRX_CTRL_0__PMU_EN;
    REG_write(REG__TRX_CTRL_0, temp); // Write updated value to register
}

// This function retrieves the latest phase measurement taken by the AT86RF233. Measurements are taken every 8us.
uint8_t AT86_getPhase(void)
{
    return REG_read(REG__PHY_PMU_VALUE); // Retrieve latest measurement from appropriate register
}

// This function sets the AT86RF233 to a state from which it can transmit a payload.
void AT86_prepareTx(void)
{
    AT86_Status_Enum status;
    do // Wait until the AT86RF233 is not transmitting anything
    {
        status = AT86_getStatus();
    } while(status == statusBUSY_TX);

    if(status == statusBUSY_RX) // Disable reception, if currently in receive mode
        AT86_sendCmd(cmdFORCE_TRX_OFF);
    AT86_sendCmd(cmdPLL_ON); // Put in mode from which it can transmit a payload
}

// This function loads a payload into the AT86RF233 TRX buffer, so that it can transmit it when commanded to.
void AT86_loadTx(const uint8_t * src, uint8_t len, uint8_t offset)
{
    SRAM_write(0, &len, 1); // Write length of payload into TRX register
    SRAM_write(offset+1, src, len); // Write payload into TRX register
}

// This function commands the AT86RF233 to transmit the payload it currently has in its TRX buffer.
void AT86_execTx(void)
{
    AT86_sendCmd(cmdTX_START); // Send AT86RF233 command to transmit payload currently in its buffer
}

// This function puts the AT86RF233 in a state from which it will receive payloads.
void AT86_prepareRx(void)
{
    irq_pending = false; // Indicate that we are addressing the payload received interrupt.
    AT86_readIstat(); // Clear pending interrupts in the AT86RF233.
    REG_write(REG__IRQ_MASK, irqRX_START|irqTRX_END); // Enable AT86RF233 interrupts when reception starts or ends.
    GPIO_clearInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN); // Reset MSP430 interrupt on the IRQ GPIO pin, so we can receive the next interrupt
    GPIO_enableInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
    AT86_sendCmd(cmdRX_ON); // Send command to enable reception
}

// This function is called to reset interrupts after we get the RX complete interrupt.
void AT86_execRx(void)
{
    irq_pending = false; // Indicate that we have addressed the interrupt
    GPIO_clearInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN); // Clear interrupt on GPIO pin so we can get the next AT86RF233 interrupt
    GPIO_enableInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN);
}

// This function retrieves the latest payload received by the AT86RF233.
void AT86_readRx(uint8_t * dest, uint8_t len, uint8_t offset)
{
    REG_write(REG__IRQ_MASK, 0); // Disable AT86RF233 interrupts, as we are done transmitting
    SRAM_read(offset, dest, len); // Read the payload that the AT86RF233 received
}

// This function reads the AT86RF233 interrupt status register so that we can tell which interrupt happened, since an IRQ pin level change indicates 1 of up to 8 possibilities.
AT86_Irq_Enum AT86_readIstat(void)
{
    irq_pending = false; // Indicate that we have dealt with the interrupt
    return (AT86_Irq_Enum) REG_read(REG__IRQ_STATUS); // Return interrupt status register
}

// This function indicates whether the AT86RF233 has issued an interrupt that has not yet been addressed (true = yes).
bool AT86_irqPending(void)
{
    return irq_pending;
}

// This interrupt happens whenever the IRQ pin has a rising edge. The AT86RF233 controls this pin, and sends a rising edge when one of its interrupts happens.
#pragma vector=PORT2_VECTOR
void __attribute__ ((interrupt)) _pinIrqHandler(void)
{
    irq_pending = true; // Indicate that there is an unaddressed interrupt
    GPIO_clearInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN); // Clear this interrupt
    GPIO_disableInterrupt(AT86_IRQ_PORT, AT86_IRQ_PIN); // Disable subsequent interrupts until this one can be addressed
}
