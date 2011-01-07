/* serial.h
 *
 * header file for serial function for ATMega32
 *
 * Author: Austin Hendrix
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <avr/io.h>

/* setup and enable serial interrupts */
void serial_init(uint8_t port);

/* set serial baud rate */
void serial_baud(uint8_t port, uint32_t baud);

/* stops the serial interrupts */
void serial_stop(uint8_t port);

/* put a byte in the transmit buffer. block until space available */
void tx_byte(uint8_t port, uint8_t b);

/* determine if there is space for another byte in the transmit buffer */
uint8_t tx_ready(uint8_t port);

/* determine if there is data in the rx buffer */
uint8_t rx_ready(uint8_t port);

/* get a byte from recieve buffer. block until data recieved */
uint8_t rx_byte(uint8_t port);

#endif
