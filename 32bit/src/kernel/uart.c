/************************************************************
 *                                                          *
 *               ~ SimpleOS - uart.c ~                       *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  PL011 UART driver for Raspberry Pi 2 and QEMU raspi2b. *
 *  Polled mode, 115200 8N1. Proven working with -serial   *
 *  stdio in QEMU.                                          *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add interrupt-driven RX/TX buffers                *
 ************************************************************/

#include <kernel/uart.h>

/** Writes a 32-bit value to an MMIO register */
void mmio_write(uint32_t reg, uint32_t data) {
    *(volatile uint32_t *)reg = data;
}

/** Reads a 32-bit value from an MMIO register */
uint32_t mmio_read(uint32_t reg) {
    return *(volatile uint32_t *)reg;
}

/** Simple cycle-count delay */
void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
                 : [count] "+r"(count) : : "cc");
}

/** Initialises the PL011 UART */
void uart_init(void) {
    /* Disable UART */
    mmio_write(UART_CR, 0);

    /* Disable pull-up/down for GPIO 14/15 */
    mmio_write(GPPUD, 0);
    delay(150);
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
    mmio_write(GPPUDCLK0, 0);

    /* Set GPIO 14/15 to ALT0 (PL011 TXD/RXD) */
    uint32_t sel1 = mmio_read(GPFSEL1);
    sel1 &= ~((7 << 12) | (7 << 15));   /* Clear bits */
    sel1 |=  (4 << 12) | (4 << 15);     /* ALT0 = 100 binary = 4 */
    mmio_write(GPFSEL1, sel1);

    /* Clear pending interrupts */
    mmio_write(UART_ICR, 0x7FF);

    /* Baud rate: integer 1, fractional ~40 → works for 115200 in QEMU raspi2b */
    mmio_write(UART_IBRD, 1);
    mmio_write(UART_FBRD, 40);

    /* Enable FIFO, 8-bit mode */
    mmio_write(UART_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    /* Mask all interrupts */
    mmio_write(UART_IMSC, 0);

    /* Enable UART, TX and RX */
    mmio_write(UART_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

/** Sends a single character (waits for TX FIFO not full) */
void uart_putc(unsigned char c) {
    while (mmio_read(UART_FR) & UART_FR_TXFF);
    mmio_write(UART_DR, c);
}

/** Receives a single character (waits for RX FIFO not empty) */
unsigned char uart_getc(void) {
    while (mmio_read(UART_FR) & UART_FR_RXFE);
    return mmio_read(UART_DR) & 0xFF;
}