/*
 * vcom.h
 *
 *  Created on: Apr 27, 2021
 *      Author: jgamm
 */

// File with function declarations for vcom.c. Specific details in this file.

#ifndef VCOM_H_
#define VCOM_H_

#include <stdint.h> // Specific definitions of integers
#include <stdbool.h> // Definition of bool data type

void   VCOM_init(void); // Initialize necessary peripherals so we can talk to computer over virtual COM port
bool   VCOM_rxAvailable(void); // Indicates whether we have received a complete command that can be read.
void   VCOM_tx(uint8_t *, uint8_t); // Transmit a string.
char * VCOM_getRxString(void); // Retrieve a complete command from the RX buffer.
bool   VCOM_isTransmitting(void); // Indicates whether we are currently transmitting over VCOM.

#endif /* VCOM_H_ */
