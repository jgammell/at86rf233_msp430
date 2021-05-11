#include <string.h>
#include <stdio.h>

#include "driverlib.h"
#include "at86.h"
#include "gpio.h"
#include "hal.h"
#include "timer_a.h"
#include "vcom.h"
#include "assert_app.h"

#define RECEIVE  ("RX")
#define TRANSMIT ("TX")

volatile AT86_Status_Enum status;
#define ADDRESS (0xAA)
#define PAYLOAD (0xFF)
uint8_t received_payload[32];

uint8_t transmit_payload[32];

volatile uint8_t phases[256];
volatile uint8_t phases_idx = 0;

#pragma vector=TIMER0_A0_VECTOR
void __attribute__ ((interrupt)) recordPhase(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, 0);
    if(phases_idx < 256)
    {
        phases[phases_idx] = AT86_getPhase();
        phases_idx += 1;
    }
}

void init(void)
{
    WDT_A_hold(WDT_A_BASE);
    PMM_setVCore(PMM_CORE_LEVEL_3);
    UCS_initClockSignal(UCS_FLLREF, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
    UCS_initClockSignal(UCS_ACLK, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
    UCS_initFLLSettle(20000U, 20000000U/UCS_REFOCLK_FREQUENCY);
    GPIO_setAsOutputPin(MCU_LED1_PORT, MCU_LED1_PIN);
    GPIO_setOutputLowOnPin(MCU_LED1_PORT, MCU_LED1_PIN);
    AT86_init();
    AT86_enablePhase(true);
    Timer_A_initUpModeParam timer_settings =
    {
     .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
     .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1,
     .timerPeriod = (16UL*UCS_getSMCLK())/1000000UL,
     .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE,
     .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
     .timerClear = TIMER_A_DO_CLEAR,
     .startTimer = false
    };
    Timer_A_initUpMode(TIMER_A0_BASE, &timer_settings);
    VCOM_init();
    __enable_interrupt();
}

void transmitPayload(uint8_t data)
{
    AT86_prepareTx();
    while(AT86_getStatus() != statusPLL_ON);
    transmit_payload[0] = ADDRESS;
    memset(transmit_payload+1, 0xFF, 31);
    AT86_loadTx(transmit_payload, 32, 0);
    AT86_execTx();
    while(AT86_getStatus() != statusBUSY_TX);
    while(AT86_getStatus() != statusPLL_ON);
    GPIO_toggleOutputOnPin(MCU_LED1_PORT, MCU_LED1_PIN);
    char msg[64];
    sprintf(msg, "(TX) Address: 0x%x, Payload: 0x%x\n", transmit_payload[0], transmit_payload[1]);
    VCOM_tx((uint8_t *)msg, strlen(msg));
}

void receivePayload(void)
{
    AT86_prepareRx();
    while((!AT86_irqPending()) && (!(AT86_readIstat() & irqRX_START)));
    phases_idx = 0;
    while((!AT86_irqPending()) && (!(AT86_readIstat() & irqTRX_END)))
    {
        phases[phases_idx] = AT86_getPhase();
        ++phases_idx;
    }
    AT86_readRx(received_payload, 4, 0);
    if((received_payload[0] == 32) && (received_payload[1]==ADDRESS))
    {
        GPIO_toggleOutputOnPin(MCU_LED1_PORT, MCU_LED1_PIN);
        char msg[64];
        sprintf(msg, "(valid RX) Length: %d, Address: 0x%x, Payload: 0x%x\n", received_payload[0], received_payload[1], received_payload[2]);
        VCOM_tx((uint8_t *)msg, strlen(msg));
        uint8_t idx;
        for(idx=0; idx<phases_idx; ++idx)
        {
            sprintf(msg, "%x\n", phases[idx]);
            while(VCOM_isTransmitting());
            VCOM_tx((uint8_t *) msg, strlen(msg));
        }
        sprintf(msg, "done\n");
        while(VCOM_isTransmitting());
        VCOM_tx((uint8_t *)msg, strlen(msg));
    }
    else
    {
        char msg[64];
        sprintf(msg, "(invalid RX) Length: %d, Address: 0x%x, Payload: 0x%x\n", received_payload[0], received_payload[1], received_payload[2]);
        VCOM_tx((uint8_t *)msg, strlen(msg));
    }
}

void parseCmd(void)
{
    char * s = VCOM_getRxString();
    char msg[2] = "\n";
    VCOM_tx((uint8_t *) msg, 1);
    if(!strcmp(s, TRANSMIT))
    {
        while(!VCOM_rxAvailable());
        s = VCOM_getRxString();
        int data;
        sscanf(s, "%d\n", &data);
        assert(data == (data&0xFF));
        transmitPayload((uint8_t) data);
    }
    else if(!strcmp(s, RECEIVE))
        receivePayload();
    else
    {
        VCOM_tx((uint8_t *)s, strlen(s));
        assert(false);
    }
}

int main(void)
{
    init();

    uint8_t part_num = AT86_getPartNum();
    assert(part_num == 0x0B);
    uint8_t version_num = AT86_getVersionNum();
    assert(version_num == 0x02);
    uint16_t man_id = AT86_getManId();
    assert(man_id == 0x001F);

    while(1)
    {
        if(VCOM_rxAvailable())
            parseCmd();
    }


    return (0);
}
