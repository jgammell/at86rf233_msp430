/*
 * vcom.h
 *
 *  Created on: Apr 27, 2021
 *      Author: jgamm
 */

#ifndef VCOM_H_
#define VCOM_H_

#include <stdint.h>
#include <stdbool.h>

void   VCOM_init(void);
bool   VCOM_rxAvailable(void);
void   VCOM_tx(uint8_t *, uint8_t);
char * VCOM_getRxString(void);
bool   VCOM_isTransmitting(void);

#endif /* VCOM_H_ */
