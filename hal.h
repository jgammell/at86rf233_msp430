/*
 * hal.h
 *
 *  Created on: Apr 10, 2021
 *      Author: jgamm
 */

#ifndef HAL_H_
#define HAL_H_

#define MCU_LED1_PORT    (GPIO_PORT_P1)
#define MCU_LED1_PIN     (GPIO_PIN0)
#define MCU_LED2_PORT    (GPIO_PORT_P4)
#define MCU_LED2_PIN     (GPIO_PIN7)
#define MCU_S1_PORT      (GPIO_PORT_P2)
#define MCU_S1_PIN       (GPIO_PIN1)
#define MCU_S2_PORT      (GPIO_PORT_P1)
#define MCU_S2_PIN       (GPIO_PIN1)
#define MCU_BCUARX_PORT  (GPIO_PORT_P4)
#define MCU_BCUARX_PIN   (GPIO_PIN5)
#define MCU_BCUATX_PORT  (GPIO_PORT_P4)
#define MCU_BCUATX_PIN   (GPIO_PIN4)
#define MCU_BCUA_UCA     (USCI_A1_BASE)

#define AT86_DIG1_PORT   (GPIO_PORT_P1)
#define AT86_DIG1_PIN    (GPIO_PIN2)
#define AT86_DIG2_PORT   (GPIO_PORT_P1)
#define AT86_DIG2_PIN    (GPIO_PIN4)
#define AT86_DIG3_PORT   (GPIO_PORT_P1)
#define AT86_DIG3_PIN    (GPIO_PIN3)
#define AT86_RESET_PORT  (GPIO_PORT_P1)
#define AT86_RESET_PIN   (GPIO_PIN5)
#define AT86_IRQ_PORT    (GPIO_PORT_P2)
#define AT86_IRQ_PIN     (GPIO_PIN4)
#define AT86_WAKEUP_PORT (GPIO_PORT_P2)
#define AT86_WAKEUP_PIN  (GPIO_PIN5)
#define AT86_SPI_BASE    (USCI_B0_BASE)
#define AT86_SPI_FREQ    (4000000U)
#define AT86_SS_PORT     (GPIO_PORT_P2)
#define AT86_SS_PIN      (GPIO_PIN3)
#define AT86_MOSI_PORT   (GPIO_PORT_P3)
#define AT86_MOSI_PIN    (GPIO_PIN0)
#define AT86_MISO_PORT   (GPIO_PORT_P3)
#define AT86_MISO_PIN    (GPIO_PIN1)
#define AT86_SCK_PORT    (GPIO_PORT_P3)
#define AT86_SCK_PIN     (GPIO_PIN2)
#define AT86_PWR_PORT    (GPIO_PORT_P2)
#define AT86_PWR_PIN     (GPIO_PIN6)


#endif /* HAL_H_ */
