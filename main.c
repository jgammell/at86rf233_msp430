// This file defines the high-level details of interactions between the computer and MSP430, and between the MSP430 and AT86RF233.

#include <string.h> // TI-provided library to work with strings
#include <stdio.h> // TI-provided library to work with strings

#include "driverlib.h" // TI-provided library to control MSP430 peripherals
#include "at86.h" // Low-level control of AT86RF233
#include "gpio.h" // TI-provided library to control MSP430 GPIO pins
#include "hal.h" // Definitions of pins/ports, peripheral initialization details, etc.
#include "timer_b.h" // TI-provided library to control hardware timer
#include "vcom.h" // Low-level control of UART to talk to computer
#include "assert_app.h" // Assert statements so we can abort code if errors happen

#define RECEIVE  ("RX") // Command computer sends to tell us to have AT86RF233 receive a payload
#define TRANSMIT ("TX") // Command computer sends to tell us to have AT86RF233 send a payload
#define CHANNEL  ("CH") // Command computer sends to tell us to change AT86RF233 channel

volatile AT86_Status_Enum status;
#define ADDRESS (0xAA) // Address included in payload so upon reception we can distinguish between payloads we sent and garbage payloads
//#define PAYLOAD (0xFF)


#define TX_PAYLOAD_LEN (64U) // Number of bytes per payload (including address byte)
uint8_t transmit_payload[TX_PAYLOAD_LEN]; // Buffer to store payload to transmit
uint8_t received_payload[TX_PAYLOAD_LEN]; // Buffer to store payload received by AT86RF233

#define NUM_PHASE_SAMPLES (256U) // Number of phase measurements to take during reception
volatile uint8_t phases[NUM_PHASE_SAMPLES]; // Buffer in which to store phase measurements
volatile uint16_t phases_idx = 0; // Index of phase measurement buffer

// This function initializes the MSP430 peripherals we will be using, as well as the AT86RF233.
void init(void)
{
    WDT_A_hold(WDT_A_BASE); // Stop MSP430 watchdog timer
    PMM_setVCore(PMM_CORE_LEVEL_3); // Put MSP430 in high-power mode to allow faster CPU clock
    UCS_initClockSignal(UCS_FLLREF, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1); // Initialize FLL as CPU clock
    UCS_initClockSignal(UCS_ACLK, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
    UCS_initFLLSettle(20000U, 20000000U/UCS_REFOCLK_FREQUENCY); // Run CPU at 20MHz
    GPIO_setAsOutputPin(MCU_LED1_PORT, MCU_LED1_PIN); // Set up LED that will toggle on transmission/reception for debugging
    GPIO_setOutputLowOnPin(MCU_LED1_PORT, MCU_LED1_PIN);
    AT86_init(); // Initialize GPIO and SPI pins going to AT86RF233, and put AT86RF233 in idle state
    AT86_enablePhase(true); // Turn on phase measurement during reception by AT86RF233
    AT86_setTxPower(0); // Have AT86RF233 transmit at 0dBm
    Timer_B_initUpModeParam timerb_settings = // Set up a timer we will use to time transmissions and receptions
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
    VCOM_init(); // Initialize UART that will let us send strings to computer over USB virtual COM port
    __enable_interrupt(); // Enable MSP430 interrupts
}

// This function has the AT86RF233 transmit a payload.
void transmitPayload(void)
{
    AT86_prepareTx(); // Put the AT86RF233 in the appropriate state for transmission.
    while(AT86_getStatus() != statusPLL_ON); // Wait until in appropriate state.
    transmit_payload[0] = ADDRESS; // Payload contains an address so upon reception we can distinguish between payloads we sent and garbage payloads.
    memset(transmit_payload+1, 0xFF, TX_PAYLOAD_LEN-1); // Payloads are hard-coded to 0xFF..., so we can measure a clean sine wave.
    AT86_loadTx(transmit_payload, TX_PAYLOAD_LEN, 0); // Prepare AT86RF233 to transmit payload by loading payload into its transmit buffer.
    HWREG16(TIMER_B0_BASE + OFS_TBxR) = 0; // Clear a timer.
    Timer_B_startCounter(TIMER_B0_BASE, TIMER_B_UP_MODE); // Start the timer so we can see how long transmission took.
    AT86_execTx(); // Transmit the payload.
    while(AT86_getStatus() != statusBUSY_TX); // Ensure it gets into the currently-transmitting state.
    while(AT86_getStatus() != statusPLL_ON); // Wait for transmission to complete.
    uint32_t time = Timer_B_getCounterValue(TIMER_B0_BASE); // Record time it took to transmit.
    Timer_B_stop(TIMER_B0_BASE); // Stop timer.
    GPIO_toggleOutputOnPin(MCU_LED1_PORT, MCU_LED1_PIN); // Toggle an LED, for debugging purposes.
    char msg[64];
    sprintf(msg, "(TX) Address: 0x%x, Time: %d us\n", transmit_payload[0], (1000000UL*time)/32768UL);
    VCOM_tx((uint8_t *)msg, strlen(msg)); // Inform computer that we have transmitted a payload.
}

// This function has the AT86RF233 wait to receive a payload, then retrieves and stores the payload.
void receivePayload(void)
{
    memset(received_payload, TX_PAYLOAD_LEN, 0); // Clear the static variable in which we will store received payload.
    AT86_prepareRx(); // Have the AT86RF233 switch into the receive state.
    while((!AT86_irqPending()) && (!(AT86_readIstat() & irqRX_START))); // Wait until AT86RF233 indicates it has started receiving something.
    HWREG16(TIMER_B0_BASE + OFS_TBxR) = 0; // Clear a timer.
    Timer_B_startCounter(TIMER_B0_BASE, TIMER_B_UP_MODE); // Start the timer to see how long reception took.
    phases_idx = 0; // Reset index for array in which we will store phase measurements.
    AT86_execRx(); // Reset interrupt so we can see when AT86RF233 is done receiving.
    while((!AT86_irqPending()) && (!(AT86_readIstat() & irqTRX_END))) // Loop until done recieving.
    {
        if(phases_idx != NUM_PHASE_SAMPLES) // Record up to this number of phases
        {
            phases[phases_idx] = AT86_getPhase(); // Retrieve and store latest phase measurement
            ++phases_idx;
        }
    }
    uint32_t time = Timer_B_getCounterValue(TIMER_B0_BASE); // Record duration of reception
    Timer_B_stop(TIMER_B0_BASE); // Stop timer.
    AT86_readRx(received_payload, 4, 0); // Retrieve the payload received by the AT86RF233
    if((received_payload[0] == TX_PAYLOAD_LEN) && (received_payload[1]==ADDRESS)) // Payload is expected length, and contained expected address
    {
        GPIO_toggleOutputOnPin(MCU_LED1_PORT, MCU_LED1_PIN); // Toggle an LED for debugging purposes
        char msg[64]; // Store retrieved payload
        sprintf(msg, "(valid RX) Length: %d, Address: 0x%x, Time: %d us\n", received_payload[0], received_payload[1], (1000000UL*time)/32768UL);
        VCOM_tx((uint8_t *)msg, strlen(msg)); // Inform computer of the received payload and reception duration
        uint16_t idx;
        for(idx=0; idx<phases_idx; ++idx) // Inform computer of the phase measurements taken during reception
        {
            sprintf(msg, "%x\n", phases[idx]);
            while(VCOM_isTransmitting()); // Wait for pending VCOM transmissions to end
            VCOM_tx((uint8_t *) msg, strlen(msg)); // Transmit the next measurement
        }
        sprintf(msg, "done\n"); // Indicate all measurements have been transmitted to computer
        while(VCOM_isTransmitting());
        VCOM_tx((uint8_t *)msg, strlen(msg));
    }
    else // Sometimes the AT86RF233 receives garbage payloads that we did not send. We can tell this is the case when the payload is not the expected length and/or does not contain the expected address.
    {
        char msg[64]; // Inform the computer that we got a garbage payload
        sprintf(msg, "(invalid RX) Length: %d, Address: 0x%x, Payload: 0x%x\n", received_payload[0], received_payload[1], received_payload[2]);
        VCOM_tx((uint8_t *)msg, strlen(msg));
    }
}

// This function interprets a command from the computer, and calls the appropriate function.
void parseCmd(void)
{
    char * s = VCOM_getRxString(); // Read an ASCII string from the computer
    char msg[2] = "\n"; // Acknowledgement string to return to computer
    VCOM_tx((uint8_t *) msg, 1); // Send acknowledgement string
    if(!strcmp(s, TRANSMIT)) // We got the transmit command
        transmitPayload(); // Have AT86RF233 transmit the payload
    else if(!strcmp(s, RECEIVE)) // We got the receive command
        receivePayload(); // Have the AT86RF233 wait to receive a payload
    else if(!strcmp(s, CHANNEL)) // We got the set channel command
    {
        while(!VCOM_rxAvailable()); // Wait until we get a string specifying the new channel
        s = VCOM_getRxString(); // Retrieve that string
        int channel;
        sscanf(s, "%d\n", &channel); // Parse it to determine the channel
        channel = channel&0x1F; // Make sure channel is only 5 bits
        AT86_setChan(channel); // Tell AT86RF233 to change to that channel
    }
    else // We got an invalid command
    {
        VCOM_tx((uint8_t *)s, strlen(s)); // Send the command back for debugging purposes
        assert(false); // Abort execution of firmware
    }
}

void main(void)
{
    // Initialize the necessary MSP peripherals and the AT86RF233
    init();

    // Verify we can talk to the AT86RF233 by verifying we can read known IDs
    uint8_t part_num = AT86_getPartNum();
    assert(part_num == 0x0B);
    uint8_t version_num = AT86_getVersionNum();
    assert(version_num == 0x02);
    uint16_t man_id = AT86_getManId();
    assert(man_id == 0x001F);

    while(1)
    {
        if(VCOM_rxAvailable()) // Wait for a command from the computer
            parseCmd(); // Execute the command
    }

}
