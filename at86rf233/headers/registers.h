/*
 * registers.h
 *
 *  Created on: Apr 10, 2021
 *      Author: jgamm
 */

// This file contains definitions of many constants defined in the AT86RF233 datasheet. Typos may exist, as only some of these are currently used in the project.
// They are named using the following convention: <type>__<register name>__<field name>
// The type can be REG for a register base address, RST for a register reset value, MASK for a bitmask of a field in the register, or SHIFT for the number of bits until the start of the field.
// Register and field names are the same as those listed in the datasheet.
// For explanations of each of these definitions, see the datasheet.

// The file also contains declarations for functions implemented in registers.c which facilitate communication over SPI with the AT86RF233, as described in the datasheet.

#ifndef AT86RF233_HEADERS_REGISTERS_H_
#define AT86RF233_HEADERS_REGISTERS_H_

#include <stdint.h>
#include <stdbool.h>


#define REG__TRX_STATUS                       (0x01)
#define RST__TRX_STATUS                       (0x00)
#define MASK__TRX_STATUS__TRX_STATUS          (0x1F)
#define MASK__TRX_STATUS__CCA_STATUS          (0x40)
#define MASK__TRX_STATUS__CCA_DONE            (0x80)
#define SHIFT__TRX_STATUS__TRX_STATUS         (0x00)
#define SHIFT__TRX_STATUS__CCA_STATUS         (0x06)
#define SHIFT__TRX_STATUS__CCA_DONE           (0x07)

#define REG__TRX_STATE                        (0x02)
#define RST__TRX_STATE                        (0x00)
#define MASK__TRX_STATE__TRX_CMD              (0x1F)
#define MASK__TRX_STATE__TRX_STATUS           (0xE0)
#define SHIFT__TRX_STATE__TRX_CMD             (0x00)
#define SHIFT__TRX_STATE__TRAC_STATUS         (0x05)

#define REG__TRX_CTRL_0                       (0x03)
#define RST__TRX_CTRL_0                       (0x09)
#define MASK__TRX_CTRL_0__CLKM_CTRL           (0x07)
#define MASK__TRX_CTRL_0__CLKM_SHA_SEL        (0x08)
#define MASK__TRX_CTRL_0__PMU_IF_INVERSE      (0x10)
#define MASK__TRX_CTRL_0__PMU_EN              (0x20)
#define MASK__TRX_CTRL_0__TOM_EN              (0x80)
#define SHIFT__TRX_CTRL_0__CLKM_CTRL          (0x00)
#define SHIFT__TRX_CTRL_0__CLKM_SHA_SEL       (0x03)
#define SHIFT__TRX_CTRL_0__PMU_IF_INVERSE     (0x04)
#define SHIFT__TRX_CTRL_0__PMU_EN             (0x05)
#define SHIFT__TRX_CTRL_0__TOM_EN             (0x07)

#define REG__TRX_CTRL_1                       (0x04)
#define RST__TRX_CTRL_1                       (0x22)
#define MASK__TRX_CTRL_1__IRQ_POLARITY        (0x01)
#define MASK__TRX_CTRL_1__IRQ_MASK_MODE       (0x02)
#define MASK__TRX_CTRL_1__SPI_CMD_MODE        (0x0C)
#define MASK__TRX_CTRL_1__RX_BL_CTRL          (0x10)
#define MASK__TRX_CTRL_1__TX_AUTO_CRC_ON      (0x20)
#define MASK__TRX_CTRL_1__IRQ_2_EXT_EN        (0x40)
#define MASK__TRX_CTRL_1__PA_EXT_EN           (0x80)
#define SHIFT__TRX_CTRL_1__IRQ_POLARITY       (0x00)
#define SHIFT__TRX_CTRL_1__IRQ_MASK_MODE      (0x01)
#define SHIFT__TRX_CTRL_1__SPI_CMD_MODE       (0x02)
#define SHIFT__TRX_CTRL_1__RX_BL_CTRL         (0x04)
#define SHIFT__TRX_CTRL_1__TX_AUTO_CRC_ON     (0x05)
#define SHIFT__TRX_CTRL_1__IRQ_2_EXT_EN       (0x06)
#define SHIFT__TRX_CTRL_1__PA_EXT_EN          (0x07)

#define REG__PHY_TX_PWR                       (0x05)
#define RST__PHY_TX_PWR                       (0x00)
#define MASK__PHY_TX_PWR__TX_PWR              (0x0F)
#define SHIFT__PHY_TX_PWR__TX_PWR             (0x00)

#define REG__PHY_RSSI                         (0x06)
#define RST__PHY_RSSI                         (0x60)
#define MASK__PHY_RSSI__RSSI                  (0x1F)
#define MASK__PHY_RSSI__RND_VALUE             (0x60)
#define MASK__PHY_RSSI__RX_CRC_VALID          (0x80)
#define SHIFT__PHY_RSSI__RSSI                 (0x00)
#define SHIFT__PHY_RSSI__RND_VALUE            (0x05)
#define SHIFT__PHY_RSSI__RX_CRC_VALID         (0x07)

#define REG__PHY_ED_LEVEL                     (0x07)
#define RST__PHY_ED_LEVEL                     (0xFF)
#define MASK__PHY_ED_LEVEL__ED_LEVEL          (0xFF)
#define SHIFT__PHY_ED_LEVEL__ED_LEVEL         (0x00)

#define REG__PHY_CC_CCA                       (0x08)
#define RST__PHY_CC_CCA                       (0x2B)
#define MASK__PHY_CC_CCA__CHANNEL             (0x1F)
#define MASK__PHY_CC_CCA__CCA_MODE            (0x60)
#define MASK__PHY_CC_CCA__CCA_REQUEST         (0x80)
#define SHIFT__PHY_CC_CCA__CHANNEL            (0x00)
#define SHIFT__PHY_CC_CCA__CCA_MODE           (0x05)
#define SHIFT__PHY_CC_CCA__CCA_REQUEST        (0x07)

#define REG__CCA_THRES                        (0x09)
#define RST__CCA_THRES                        (0xC7)
#define MASK__CCA_THRES__CCA_ED_THRES         (0x0F)
#define SHIFT__CCA_THRES__CCA_ED_THRES        (0x00)

#define REG__RX_CTRL                          (0x0A)
#define RST__RX_CTRL                          (0x37)
#define MASK__RX_CTRL__PDT_THRES              (0x0F)
#define MASK__RX_CTRL__PEL_SHIFT_VALUE        (0xC0)
#define SHIFT__RX_CTRL__PDT_THRES             (0x00)
#define SHIFT__RX_CTRL__PEL_SHIFT_VALUE       (0x06)

#define REG__SFD_VALUE                        (0x0B)
#define RST__SFD_VALUE                        (0xA7)
#define MASK__SFD_VALUE__SFD_VALUE            (0xFF)
#define SHIFT__SFD_VALUE__SFD_VALUE           (0x00)

#define REG__TRX_CTRL_2                       (0x0C)
#define RST__TRX_CTRL_2                       (0x20)
#define MASK__TX_CTRL_2__OQPSK_DATA_RATE      (0x07)
#define MASK__TX_CTRL_2__OQPSK_SCRAM_EN       (0x20)
#define MASK__TX_CTRL_2__RX_SAFE_MODE         (0x80)
#define SHIFT__TRX_CTRL_2__OQPSK_DATA_RATE    (0x00)
#define SHIFT__TRX_CTRL_2__OQPSK_SCRAM_EN     (0x05)
#define SHIFT__TRX_CTRL_2__RX_SAFE_MODE       (0x07)

#define REG__ANT_DIV                          (0x0D)
#define RST__ANT_DIV                          (0x00)
#define MASK__ANT_DIV__ANT_CTRL               (0x03)
#define MASK__ANT_DIV__ANT_EXT_SW_EN          (0x04)
#define MASK__ANT_DIV__ANT_DIV_EN             (0x08)
#define MASK__ANT_DIV__ANT_SEL                (0x80)
#define SHIFT__ANT_DIV__ANT_CTRL              (0x00)
#define SHIFT__ANT_DIV__ANT_EXT_SW_EN         (0x02)
#define SHIFT__ANT_DIV__ANT_DIV_EN            (0x03)
#define SHIFT__ANT_DIV__ANT_SEL               (0x07)

#define REG__IRQ_MASK                         (0x0E)
#define RST__IRQ_MASK                         (0x00)
#define MASK__IRQ_MASK__IRQ_MASK              (0xFF)
#define SHIFT__IRQ_MASK__IRQ_MASK             (0x00)

#define REG__IRQ_STATUS                       (0x0F)
#define RST__IRQ_STATUS                       (0x00)
#define MASK__IRQ_STATUS__IRQ_0_PLL_LOCK      (0x01)
#define MASK__IRQ_STATUS__IRQ_1_PLL_UNLOCK    (0x02)
#define MASK__IRQ_STATUS__IRQ_2_RX_START      (0x04)
#define MASK__IRQ_STATUS__IRQ_3_TRX_END       (0x08)
#define MASK__IRQ_STATUS__IRQ_4_CCA_ED_DONE   (0x10)
#define MASK__IRQ_STATUS__IRQ_5_AMI           (0x20)
#define MASK__IRQ_STATUS__IRQ_6_TRX_UR        (0x40)
#define MASK__IRQ_STATUS__IRQ_7_BAT_LOW       (0x80)
#define SHIFT__IRQ_STATUS__IRQ_0_PLL_LOCK     (0x00)
#define SHIFT__IRQ_STATUS__IRQ_1_PLL_UNLOCK   (0x01)
#define SHIFT__IRQ_STATUS__IRQ_2_RX_START     (0x02)
#define SHIFT__IRQ_STATUS__IRQ_3_TRX_END      (0x03)
#define SHIFT__IRQ_STATUS__IRQ_4_CCA_ED_DONE  (0x04)
#define SHIFT__IRQ_STATUS__IRQ_5_AMI          (0x05)
#define SHIFT__IRQ_STATUS__IRQ_6_TRX_UR       (0x06)
#define SHIFT__IRQ_STATUS__IRQ_7_BAT_LOW      (0x07)

#define REG__VREG_CTRL                        (0x10)
#define RST__VREG_CTRL                        (0x00)
#define MASK__VREG_CTRL__DVDD_OK              (0x04)
#define MASK__VREG_CTRL__DVREG_EXT            (0x08)
#define MASK__VREG_CTRL__AVDD_OK              (0x40)
#define MASK__VREG_CTRL__AVREG_EXT            (0x80)
#define SHIFT__VREG_CTRL__DVDD_OK             (0x03)
#define SHIFT__VREG_CTRL__DVREG_EXT           (0x04)
#define SHIFT__VREG_CTRL__AVDD_OK             (0x06)
#define SHIFT__VREG_CTRL__AVREG_EXT           (0x07)

#define REG__BATMON                           (0x11)
#define RST__BATMON                           (0x02)
#define MASK__BATMON__BATMON_VTH              (0x0F)
#define MASK__BATMON__BATMON_HR               (0x10)
#define MASK__BATMON__BATMON_OK               (0x20)
#define SHIFT__BATMON__BATMON_VTH             (0x00)
#define SHIFT__BATMON__BATMON_HR              (0x04)
#define SHIFT__BATMON__BATMON_OK              (0x05)

#define REG__XOSC_CTRL                        (0x12)
#define RST__XOSC_CTRL                        (0xF0)
#define MASK__XOSC_CTRL__XTAL_TRIM            (0x0F)
#define MASK__XOSC_CTRL__XTAL_MODE            (0xF0)
#define SHIFT__XOSC_CTRL__XTAL_TRIM           (0x00)
#define SHIFT__XOSC_CTRL__XTAL_MODE           (0x04)

#define REG__CC_CTRL_0                        (0x13)
#define RST__CC_CTRL_0                        (0x00)
#define MASK__CC_CTRL_0__CC_NUMBER            (0xFF)
#define SHIFT__CC_CTRL_0__CC_NUMBER           (0x00)

#define REG__CC_CTRL_1                        (0x14)
#define RST__CC_CTRL_1                        (0x00)
#define MASK__CC_CTRL_1__CC_BAND              (0x0F)
#define SHIFT__CC_CTRL_1__CC_BAND             (0x00)

#define REG__RX_SYN                           (0x15)
#define RST__RX_SYN                           (0x00)
#define MASK__RX_SYN__RX_PDT_LEVEL            (0x0F)
#define MASK__RX_SYN__RX_PDT_DIS              (0x80)
#define SHIFT__RX_SYN__RX_PDT_LEVEL           (0x00)
#define SHIFT__RX_SYN__RX_PDT_DIS             (0x07)

#define REG__TRX_RPC                          (0x16)
#define RST__TRX_RPC                          (0xC1)
#define MASK__TRX_RPC__IPAN_RPC_EN            (0x02)
#define MASK__TRX_RPC__XAH_TX_RPC_EN          (0x04)
#define MASK__TRX_RPC__PLL_RPC_EN             (0x08)
#define MASK__TRX_RPC__PDT_RPC_EN             (0x10)
#define MASK__TRX_RPC__RX_RPC_EN              (0x20)
#define MASK__TRX_RPC_CTRL                    (0xC0)
#define SHIFT__TRX_RPC__IPAN_RPC_EN           (0x01)
#define SHIFT__TRX_RPC__XAH_TX_RPC_EN         (0x02)
#define SHIFT__TRX_RPC__PLL_RPC_EN            (0x03)
#define SHIFT__TRX_RPC__PDT_RPC_EN            (0x04)
#define SHIFT__TRX_RPC__RX_RPC_EN             (0x05)
#define SHIFT__TRX_RPC__RX_RPC_CTRL           (0x06)

#define REG__XAH_CTRL_1                       (0x17)
#define RST__XAH_CTRL_1                       (0x00)
#define MASK__XAH_CTRL_1__AACK_SPC_EN         (0x01)
#define MASK__XAH_CTRL_1__AACK_PROM_MODE      (0x02)
#define MASK__XAH_CTRL_1__AACK_ACK_TIME       (0x04)
#define MASK__XAH_CTRL_1__AACK_UPLD_RES_FT    (0x10)
#define MASK__XAH_CTRL_1__AACK_FLTR_RES_FT    (0x20)
#define MASK__XAH_CTRL_1__ARET_TX_TS_EN       (0x80)
#define SHIFT__XAH_CTRL_1__AACK_SPC_EN        (0x00)
#define SHIFT__XAH_CTRL_1__AACK_PROM_MODE     (0x01)
#define SHIFT__XAH_CTRL_1__AACK_ACK_TIME      (0x02)
#define SHIFT__XAH_CTRL_1__AACK_UPLD_RES_FT   (0x04)
#define SHIFT__XAH_CTRL_1__AACK_FLTR_RES_FT   (0x05)
#define SHIFT__XAH_CTRL_1__ARET_TX_TS_EN      (0x07)

#define REG__FTN_CTRL                         (0x18)
#define RST__FTN_CTRL                         (0x58)
#define MASK__FTN_CTRL__FTNV                  (0x3F)
#define MASK__FTN_CTRL__FTN_START             (0x80)
#define SHIFT__FTN_CTRL__FTNV                 (0x00)
#define SHIFT__FTN_CTRL__FTN_START            (0x07)

#define REG__XAH_CTRL_2                       (0x19)
#define RST__XAH_CTRL_2                       (0x00)
#define MASK__XAH_CTRL_2__ARET_CSMA_RETRIES   (0x0E)
#define MASK__XAH_CTRL_2__ARET_FRAME_RETRIES  (0xF0)
#define SHIFT__XAH_CTRL_2__ARET_CSMA_RETRIES  (0x01)
#define SHIFT__XAH_CTRL_2__ARET_FRAME_RETRIES (0x04)

#define REG__PLL_CF                           (0x1A)
#define RST__PLL_CF                           (0x57)
#define MASK__PLL_CF__PLL_CF                  (0x0F)
#define MASK__PLL_CF__PLL_CF_START            (0x80)
#define SHIFT__PLL_CF__PLL_CF                 (0x00)
#define SHIFT__PLL_CF__PLL_CF_START           (0x07)

#define REG__PLL_DCU                          (0x1B)
#define RST__PLL_DCU                          (0x20)
#define MASK__PLL_DCU__PLL_DCU_START          (0x80)
#define SHIFT__PLL_DCU__PLL_DCU_START         (0x07)

#define REG__PART_NUM                         (0x1C)
#define RST__PART_NUM                         (0x0B)
#define MASK__PART_NUM__PART_NUM              (0xFF)
#define SHIFT__PART_NUM__PART_NUM             (0x00)

#define REG__VERSION_NUM                      (0x1D)
#define RST__VERSION_NUM_A                    (0x01)
#define RST__VERSION_NUM_B                    (0x02)
#define MASK__VERSION_NUM__VERSION_NUM        (0xFF)
#define SHIFT__VERSION_NUM__VERSION_NUM       (0x00)

#define REG__MAN_ID_0                         (0x1E)
#define RST__MAN_ID_0                         (0x1F)
#define MASK__MAN_ID_0__MAN_ID_0              (0xFF)
#define SHIFT__MAN_ID_0__MAN_ID_0             (0x00)

#define REG__MAN_ID_1                         (0x1F)
#define RST__MAN_ID_1                         (0x00)
#define MASK__MAN_ID_1__MAN_ID_1              (0xFF)
#define SHIFT__MAN_ID_1__MAN_ID_1             (0x00)

#define REG__SHORT_ADDR_0                     (0x20)
#define RST__SHORT_ADDR_0                     (0xFF)
#define MASK__SHORT_ADDR_0__SHORT_ADDR_0      (0xFF)
#define SHIFT__SHORT_ADDR_0__SHORT_ADDR_0     (0x00)

#define REG__SHORT_ADDR_1                     (0x21)
#define RST__SHORT_ADDR_1                     (0xFF)
#define MASK__SHORT_ADDR_1__SHORT_ADDR_1      (0xFF)
#define SHIFT__SHORT_ADDR_1__SHORT_ADDR_1     (0x00)

#define REG__PAN_ID_0                         (0x22)
#define RST__PAN_ID_0                         (0xFF)
#define MASK__PAN_ID_0__PAN_ID_0              (0xFF)
#define SHIFT__PAN_ID_0__PAN_ID_0             (0x00)

#define REG__PAN_ID_1                         (0x23)
#define RST__PAN_ID_1                         (0xFF)
#define MASK__PAN_ID_1__PAN_ID_1              (0xFF)
#define SHIFT__PAN_ID_1__PAN_ID_1             (0x00)

#define REG__IEEE_ADDR_0                      (0x24)
#define RST__IEEE_ADDR_0                      (0x00)
#define MASK__IEEE_ADDR_0__IEEE_ADDR_0        (0xFF)
#define SHIFT__IEEE_ADDR_0__IEEE_ADDR_0       (0x00)

#define REG__IEEE_ADDR_1                      (0x25)
#define RST__IEEE_ADDR_1                      (0x00)
#define MASK__IEEE_ADDR_1__IEEE_ADDR_1        (0xFF)
#define SHIFT__IEEE_ADDR_1__IEEE_ADDR_1       (0x00)

#define REG__IEEE_ADDR_2                      (0x26)
#define RST__IEEE_ADDR_2                      (0x00)
#define MASK__IEEE_ADDR_2__IEEE_ADDR_2        (0xFF)
#define SHIFT__IEEE_ADDR_2__IEEE_ADDR_2       (0x00)

#define REG__IEEE_ADDR_3                      (0x27)
#define RST__IEEE_ADDR_3                      (0x00)
#define MASK__IEEE_ADDR_3__IEEE_ADDR_3        (0xFF)
#define SHIFT__IEEE_ADDR_3__IEEE_ADDR_3       (0x00)

#define REG__IEEE_ADDR_4                      (0x28)
#define RST__IEEE_ADDR_4                      (0x00)
#define MASK__IEEE_ADDR_4__IEEE_ADDR_4        (0xFF)
#define SHIFT__IEEE_ADDR_4__IEEE_ADDR_4       (0x00)

#define REG__IEEE_ADDR_5                      (0x29)
#define RST__IEEE_ADDR_5                      (0x00)
#define MASK__IEEE_ADDR_5__IEEE_ADDR_5        (0xFF)
#define SHIFT__IEEE_ADDR_5__IEEE_ADDR_5       (0x00)

#define REG__IEEE_ADDR_6                      (0x2A)
#define RST__IEEE_ADDR_6                      (0x00)
#define MASK__IEEE_ADDR_6__IEEE_ADDR_6        (0xFF)
#define SHIFT__IEEE_ADDR_6__IEEE_ADDR_6       (0x00)

#define REG__IEEE_ADDR_7                      (0x2B)
#define RST__IEEE_ADDR_7                      (0x00)
#define MASK__IEEE_ADDR_7__IEEE_ADDR_7        (0xFF)
#define SHIFT__IEEE_ADDR_7__IEEE_ADDR_7       (0x00)

#define REG__XAH_CTRL_0                       (0x2C)
#define RST__XAH_CTRL_0                       (0x38)
#define MASK__XAH_CTRL_0__SLOTTED_OPERATION   (0x01)
#define MASK__XAH_CTRL_0__MAX_CSMA_RETRIES    (0x0E)
#define MASK__XAH_CTRL_0__MAX_FRAME_RETRIES   (0xF)
#define SHIFT__XAH_CTRL_0__SLOTTED_OPERATION  (0x00)
#define SHIFT__XAH_CTRL_0__MAX_CSMA_RETRIES   (0x01)
#define SHIFT__XAH_CTRL_0__MAX_FRAME_RETRIES  (0x04)

#define REG__CSMA_SEED_0                      (0x2D)
#define RST__CSMA_SEED_0                      (0xEA)
#define MASK__CSMA_SEED_0__CSMA_SEED_0        (0xFF)
#define SHIFT__CSMA_SEED_0__CSMA_SEED_0       (0x00)

#define REG__CSMA_SEED_1                      (0x2E)
#define RST__CSMA_SEED_1                      (0x42)
#define MASK__CSMA_SEED_1__CSMA_SEED_1        (0x07)
#define MASK__CSMA_SEED_1__AACK_I_AM_COORD    (0x08)
#define MASK__CSMA_SEED_1__AACK_DIS_ACK       (0x10)
#define MASK__CSMA_SEED_1__AACK_SET_PD        (0x20)
#define MASK__CSMA_SEED_1__AACK_FVN_MODE      (0xC0)
#define SHIFT__CSMA_SEED_1__CSMA_SEED_1       (0x00)
#define SHIFT__CSMA_SEED_1__AACK_I_AM_COORD   (0x03)
#define SHIFT__CSMA_SEED_1__AACK_DIS_ACK      (0x04)
#define SHIFT__CSMA_SEED_1__AACK_SET_PD       (0x05)
#define SHIFT__CSMA_SEED_1__AACK_FVN_MODE     (0x06)

#define REG__CSMA_BE                          (0x2F)
#define RST__CSMA_BE                          (0x53)
#define MASK__CSMA_BE__MIN_BE                 (0x0F)
#define MASK__CSMA_BE__MAX_BE                 (0xF0)
#define SHIFT__CSMA_BE__MIN_BE                (0x00)
#define SHIFT__CSMA_BE__MAX_BE                (0x04)

#define REG__TST_CTRL_DIGI                    (0x36)
#define RST__TST_CTRL_DIGI                    (0x00)
#define MASK__TST_CTRL_DIGI__TST_CTRL_DIG     (0x07)
#define SHIFT__TST_CTRL_DIGI__TST_CTRL_DIG    (0x00)

#define REG__TST_AGC                          (0x3C)
#define RST__TST_AGC                          (0x00)
#define MASK__TST_AGC__GC                     (0x03)
#define MASK__TST_AGC__AGC_HOLD               (0x04)
#define MASK__TST_AGC__AGC_OFF                (0x08)
#define MASK__TST_AGC__AGC_RST                (0x10)
#define MASK__TST_AGC__AGC_HOLD_SEL           (0x20)
#define SHIFT__TST_AGC__GC                    (0x00)
#define SHIFT__TST_AGC__AGC_HOLD              (0x02)
#define SHIFT__TST_AGC__AGC_OFF               (0x03)
#define SHIFT__TST_AGC__AGC_RST               (0x04)
#define SHIFT__TST_AGC__AGC_HOLD_SEL          (0x05)

#define REG__TST_SDM                          (0x3D)
#define RST__TST_SDM                          (0x00)
#define MASK__TST_SDM__TX_RX_SEL              (0x10)
#define MASK__TST_SDM__TX_RX                  (0x20)
#define MASK__TST_SDM__MOD                    (0x40)
#define MASK__TST_SDM__MOD_SEL                (0x80)
#define SHIFT__TST_SDM__TX_RX_SEL             (0x04)
#define SHIFT__TST_SDM__TX_RX                 (0x05)
#define SHIFT__TST_SDM__MOD                   (0x06)
#define SHIFT__TST_SDM__MOD_SEL               (0x07)

#define REG__PHY_TX_TIME                      (0x3B)
#define RST__PHY_TX_TIME                      (0x00)
#define MASK__PHY_TX_TIME__IRC_TX_TIME        (0x0F)
#define SHIFT__PHY_TX_TIME_IRC_TX_TIME        (0x00)

#define REG__PHY_PMU_VALUE                    (0x3B)
#define RST__PHY_PMU_VALUE                    (0x00)
#define MASK__PHY_PMU_VALUE__PMU_VALUE        (0xFF)
#define SHIFT__PHY_PMU_VALUE__PMU_VALUE       (0x00)

void    SPI_init(void); // Initializes MSP430 SPI peripheral that will be used to talk to AT86RF233

void    REG_write(uint8_t address, uint8_t value); // Writes a value to one of the AT86RF233 registers.

uint8_t REG_read(uint8_t address); // Reads the value of one of the AT86RF233 registers.

void    SRAM_read(uint8_t offset, uint8_t * dest, uint8_t len); // Reads part of the TRX buffer in the AT86RF233.

void    SRAM_write(uint8_t offset, const uint8_t * src, uint8_t len); // Writes to part of the TRX buffer in the AT86RF233.

void    FB_read(uint8_t * dest, uint8_t len); // Reads from the beginning of the TRX buffer in the AT86RF233.


#endif /* AT86RF233_HEADERS_REGISTERS_H_ */
