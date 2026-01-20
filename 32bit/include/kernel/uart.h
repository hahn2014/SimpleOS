/************************************************************
 *                                                          *
 *                 ~ SimpleOS - uart.h ~                     *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Declarations for the PL011 UART driver (Raspberry Pi 2  *
 *  and QEMU raspi2b). Polled 115200 8N1 on GPIO 14/15 ALT0.*
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add interrupt-driven buffering                    *
 ************************************************************/

#ifndef UART_H
#define UART_H

#include <stdint.h>

/* PL011 UART base (Pi2/Pi3/QEMU raspi2b) */
#define UART_BASE       0x3F201000UL

/* PL011 registers */
#define UART_DR         (UART_BASE + 0x00UL)
#define UART_RSRECR     (UART_BASE + 0x04UL)
#define UART_FR         (UART_BASE + 0x18UL)
#define UART_ILPR       (UART_BASE + 0x20UL)
#define UART_IBRD       (UART_BASE + 0x24UL)
#define UART_FBRD       (UART_BASE + 0x28UL)
#define UART_LCRH       (UART_BASE + 0x2CUL)
#define UART_CR         (UART_BASE + 0x30UL)
#define UART_IFLS       (UART_BASE + 0x34UL)
#define UART_IMSC       (UART_BASE + 0x38UL)
#define UART_RIS        (UART_BASE + 0x3CUL)
#define UART_MIS        (UART_BASE + 0x40UL)
#define UART_ICR        (UART_BASE + 0x44UL)
#define UART_DMACR      (UART_BASE + 0x48UL)

/* Flag register bits */
#define UART_FR_TXFE    (1UL << 7)   /* Transmit FIFO empty */
#define UART_FR_TXFF    (1UL << 5)   /* Transmit FIFO full */
#define UART_FR_RXFE    (1UL << 4)   /* Receive FIFO empty */

/* GPIO registers (for ALT0 setup) */
#define GPIO_BASE       0x3F200000UL
#define GPFSEL1         (GPIO_BASE + 0x04UL)
#define GPPUD           (GPIO_BASE + 0x94UL)
#define GPPUDCLK0       (GPIO_BASE + 0x98UL)

/** Initialises PL011 UART (115200 8N1) and configures GPIO 14/15 to ALT0 */
void uart_init(void);

/** Sends a single character (blocking polled) */
void uart_putc(unsigned char c);

/** Receives a single character (blocking polled) */
unsigned char uart_getc(void);

/** Writes a 32-bit value to an MMIO register */
void mmio_write(uint32_t reg, uint32_t data);

/** Reads a 32-bit value from an MMIO register */
uint32_t mmio_read(uint32_t reg);

/** Simple busy-wait delay used during GPIO/UART setup */
void delay(int32_t count);

#endif /* UART_H */