#include <string.h>
#include <stdio.h>

#include "driverlib.h"
#include "at86.h"
#include "gpio.h"
#include "hal.h"
#include "timer_b.h"
#include "vcom.h"
#include "assert_app.h"

#define RECEIVE  ("RX")
#define TRANSMIT ("TX")
#define CHANNEL  ("CH")

volatile AT86_Status_Enum status;
#define ADDRESS (0xAA)
#define PAYLOAD (0xFF)


#define TX_PAYLOAD_LEN (64U)
uint8_t transmit_payload[TX_PAYLOAD_LEN];
uint8_t received_payload[TX_PAYLOAD_LEN];

#define NUM_PHASE_SAMPLES (256U)
volatile uint8_t phases[NUM_PHASE_SAMPLES];
volatile uint16_t phases_idx = 0;

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
    AT86_setTxPower(0);
    Timer_B_initUpModeParam timerb_settings =
    {
     .clockSource = TIMER_B_CLOCKSOURCE_ACLK,
     .clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1,
     .timerPeriod = 0xFFFF,
     .timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE,
     .captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE,
     .timerClear = TIMER_B_DO_CLEAR,
     .startTimer = false
    };
    Timer_B_initUpMode(TIMER_B0_BASE, &timerb_settings);
    VCOM_init();
    __enable_interrupt();
}

void transmitPayload(void)
{
    AT86_prepareTx();
    while(AT86_getStatus() != statusPLL_ON);
    transmit_payload[0] = ADDRESS;
    memset(transmit_payload+1, 0xFF, TX_PAYLOAD_LEN-1);
    AT86_loadTx(transmit_payload, TX_PAYLOAD_LEN, 0);
    HWREG16(TIMER_B0_BASE + OFS_TBxR) = 0;
    Timer_B_startCounter(TIMER_B0_BASE, TIMER_B_UP_MODE);
    AT86_execTx();
    while(AT86_getStatus() != statusBUSY_TX);
    while(AT86_getStatus() != statusPLL_ON);
    uint32_t time = Timer_B_getCounterValue(TIMER_B0_BASE);
    Timer_B_stop(TIMER_B0_BASE);
    GPIO_toggleOutputOnPin(MCU_LED1_PORT, MCU_LED1_PIN);
    char msg[64];
    sprintf(msg, "(TX) Address: 0x%x, Time: %d us\n", transmit_payload[0], (1000000UL*time)/32768UL);
    VCOM_tx((uint8_t *)msg, strlen(msg));
}

void receivePayload(void)
{
    memset(received_payload, TX_PAYLOAD_LEN, 0);
    AT86_prepareRx();
    while((!AT86_irqPending()) && (!(AT86_readIstat() & irqRX_START)));
    HWREG16(TIMER_B0_BASE + OFS_TBxR) = 0;
    Timer_B_startCounter(TIMER_B0_BASE, TIMER_B_UP_MODE);
    phases_idx = 0;
    AT86_execRx();
    while((!AT86_irqPending()) && (!(AT86_readIstat() & irqTRX_END)))
    {
        if(phases_idx != NUM_PHASE_SAMPLES)
        {
            phases[phases_idx] = AT86_getPhase();
            ++phases_idx;
        }
    }
    uint32_t time = Timer_B_getCounterValue(TIMER_B0_BASE);
    Timer_B_stop(TIMER_B0_BASE);
    AT86_readRx(received_payload, 4, 0);
    if((received_payload[0] == TX_PAYLOAD_LEN) && (received_payload[1]==ADDRESS))
    {
        GPIO_toggleOutputOnPin(MCU_LED1_PORT, MCU_LED1_PIN);
        char msg[64];
        sprintf(msg, "(valid RX) Length: %d, Address: 0x%x, Time: %d us\n", received_payload[0], received_payload[1], (1000000UL*time)/32768UL);
        VCOM_tx((uint8_t *)msg, strlen(msg));
        uint16_t idx;
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
        transmitPayload();
    else if(!strcmp(s, RECEIVE))
        receivePayload();
    else if(!strcmp(s, CHANNEL))
    {
        while(!VCOM_rxAvailable());
        s = VCOM_getRxString();
        int channel;
        sscanf(s, "%d\n", &channel);
        channel = channel&0x1F;
        AT86_setChan(channel);
    }
    else
    {
        VCOM_tx((uint8_t *)s, strlen(s));
        assert(false);
    }
}

void main(void)
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

}
