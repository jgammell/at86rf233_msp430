/*
 * hal.h
 *
 *  Created on: Apr 10, 2021
 *      Author: jgamm
 */

// This file contains pin definitions and details for initializing peripherals.

#ifndef HAL_H_
#define HAL_H_

#define MCU_LED1_PORT    (GPIO_PORT_P1) // LED 1 on the MSP-EXP430F5529LP
#define MCU_LED1_PIN     (GPIO_PIN0)
#define MCU_LED2_PORT    (GPIO_PORT_P4) // LED 2 on the MSP-EXP430F5529LP
#define MCU_LED2_PIN     (GPIO_PIN7)
#define MCU_S1_PORT      (GPIO_PORT_P2) // Button 1 on the MSP-EXP430F5529LP
#define MCU_S1_PIN       (GPIO_PIN1)
#define MCU_S2_PORT      (GPIO_PORT_P1) // Button 2 on the MSP-EXP430F5529LP
#define MCU_S2_PIN       (GPIO_PIN1)
#define MCU_BCUARX_PORT  (GPIO_PORT_P4) // UART RX pin we will be using on the MSP-EXP430F5529LP
#define MCU_BCUARX_PIN   (GPIO_PIN5)
#define MCU_BCUATX_PORT  (GPIO_PORT_P4) // UART TX pin we will be using on the MSP-EXP430F5529LP
#define MCU_BCUATX_PIN   (GPIO_PIN4)
#define MCU_BCUA_UCA     (USCI_A1_BASE) // Base address of the memory-mapped control registers for the UART we will be using

#define AT86_DIG1_PORT   (GPIO_PORT_P1) // MSP-EXP430F5529LP GPIO pin we will attach to DIG1 pin of the AT86RF233
#define AT86_DIG1_PIN    (GPIO_PIN2)
#define AT86_DIG2_PORT   (GPIO_PORT_P1) // MSP-EXP430F5529LP GPIO pin we will attach to the DIG2 pin of the AT86RF233
#define AT86_DIG2_PIN    (GPIO_PIN4)
#define AT86_DIG3_PORT   (GPIO_PORT_P1) // MSP-EXP430F5529LP GPIO pin we will attach to the DIG3 pin of the AT86RF233
#define AT86_DIG3_PIN    (GPIO_PIN3)
#define AT86_RESET_PORT  (GPIO_PORT_P1) // MSP-EXP430F5529LP GPIO pin we will attach to the RESET pin of the AT86RF233
#define AT86_RESET_PIN   (GPIO_PIN5)
#define AT86_IRQ_PORT    (GPIO_PORT_P2) // MSP-EXP430F5529LP GPIO pin we will attach to the IRQ pin of the AT86RF233
#define AT86_IRQ_PIN     (GPIO_PIN4)
#define AT86_WAKEUP_PORT (GPIO_PORT_P2) // MSP-EXP430F5529LP GPIO pin we will attach to the WAKEUP pin of the AT86RF233
#define AT86_WAKEUP_PIN  (GPIO_PIN5)
#define AT86_SPI_BASE    (USCI_B0_BASE) // Base address of the memory-mapped control registers of the MSP430F5529LP SPI module we will be using
#define AT86_SPI_FREQ    (6500000U) // Frequency (Hz) at which we will run the SPI module
#define AT86_SS_PORT     (GPIO_PORT_P2) // MSP-EXP430F5529LP GPIO pin we will attach to the SS pin of the AT86RF233
#define AT86_SS_PIN      (GPIO_PIN3)
#define AT86_MOSI_PORT   (GPIO_PORT_P3) // MSP-EXP430F5529LP MOSI pin we will attach to the MOSI pin of the AT86RF233
#define AT86_MOSI_PIN    (GPIO_PIN0)
#define AT86_MISO_PORT   (GPIO_PORT_P3) // MSP-EXP430F5529LP MISO pin we will attach to the MISO pin of the AT86RF233
#define AT86_MISO_PIN    (GPIO_PIN1)
#define AT86_SCK_PORT    (GPIO_PORT_P3) // MSP-EXP430F5529 SPI clock pin we will attach to the SCK pin of the AT86RF233
#define AT86_SCK_PIN     (GPIO_PIN2)
#define AT86_PWR_PORT    (GPIO_PORT_P2) // MSP-EXP430F5529LP GPIO pin we will use to toggle power to the AT86RF233
#define AT86_PWR_PIN     (GPIO_PIN6)


#endif /* HAL_H_ */
