/************************************************************
 *                                                          *
 *                ~ SimpleOS - uart.c ~                     *
 *                                                          *
 *  Fully interrupt-driven PL011 UART with ring buffers.    *
 *  Low-power blocking I/O (WFI when waiting).              *
 *  Replaces polling completely.                            *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 ************************************************************/

#include <kernel/uart.h>
#include <common/stdio.h>
#include <kernel/mmio.h>
#include <stddef.h> // For size_t (ring buffer indices)

/* Power-of-2 buffers for efficient indexing */
#define RX_BUF_SIZE      512
#define TX_BUF_SIZE      2048

#define ENABLE_IRQS_2    0x3F00B214UL
#define UART0_IRQ        (1 << 25)      /* PL011 interrupt in bank 2 */

static uint8_t rx_buf[RX_BUF_SIZE];
static volatile size_t rx_head = 0;
static volatile size_t rx_tail = 0;

static uint8_t tx_buf[TX_BUF_SIZE];
static volatile size_t tx_head = 0;
static volatile size_t tx_tail = 0;

static inline bool rx_empty(void) { return rx_head == rx_tail; }
static inline bool tx_empty(void) { return tx_head == tx_tail; }
static inline bool tx_full(void)  { return ((tx_head + 1) & (TX_BUF_SIZE - 1)) == tx_tail; }

/* Internal helper to prime TX FIFO and enable TX interrupt if needed */
static void start_tx(void) {
    /* Try to fill FIFO directly first (low latency) */
    while (!tx_empty() && !(mmio_read(UART_FR) & UART_FR_TXFF)) {
        mmio_write(UART_DR, tx_buf[tx_tail]);
        tx_tail = (tx_tail + 1) & (TX_BUF_SIZE - 1);
    }

    /* If still data pending, enable TX interrupt */
    if (!tx_empty()) {
        mmio_write(UART_IMSC, mmio_read(UART_IMSC) | (1 << 6));  /* TXIM */
    }
}

/** Initialises the PL011 UART - now interrupt-driven */
void uart_init(void) {
    /* Disable UART */
    mmio_write(UART_CR, 0);

    /* Disable pull-up/down for GPIO 14/15 */
    mmio_write(GPPUD, 0);
    { volatile unsigned int __delay; for (__delay = 0; __delay < 150; __delay++) asm volatile("nop"); }
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    { volatile unsigned int __delay; for (__delay = 0; __delay < 150; __delay++) asm volatile("nop"); }
    mmio_write(GPPUDCLK0, 0);

    /* Set GPIO 14/15 to ALT0 (PL011 TXD/RXD) */
    uint32_t sel1 = mmio_read(GPFSEL1);
    sel1 &= ~((7 << 12) | (7 << 15));
    sel1 |=  (4 << 12) | (4 << 15);     /* ALT0 */
    mmio_write(GPFSEL1, sel1);

    /* Clear pending interrupts */
    mmio_write(UART_ICR, 0x7FF);

    /* Baud rate: 115200 (works well in QEMU raspi2b and real Pi2/Pi3) */
    mmio_write(UART_IBRD, 1);
    mmio_write(UART_FBRD, 40);

    /* Enable FIFO, 8-bit mode */
    mmio_write(UART_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    /* Mask all interrupts initially */
    mmio_write(UART_IMSC, 0);

    /* Enable UART, TX and RX */
    mmio_write(UART_CR, (1 << 0) | (1 << 8) | (1 << 9));

    /* Clear ring buffers */
    rx_head = rx_tail = tx_head = tx_tail = 0;

    /* Enable RX, Receive Timeout, and TX interrupts */
    mmio_write(UART_IMSC, (1 << 4) | (1 << 5) | (1 << 6));  /* RXIM | RTIM | TXIM */

    /* Enable PL011 UART interrupt in GPU interrupt controller */
    mmio_write(ENABLE_IRQS_2, mmio_read(ENABLE_IRQS_2) | UART0_IRQ);

    info("PL011 UART initialized - interrupt-driven async I/O ready");
}

/** UART IRQ handler - drains RX FIFO, fills TX FIFO */
void uart_irq_handler(void) {
    uint32_t mis = mmio_read(UART_MIS);

    /* RX or Receive Timeout pending */
    if (mis & ((1 << 4) | (1 << 5))) {
        while (!(mmio_read(UART_FR) & (1 << 4))) {  /* !RXFE */
            uint8_t c = mmio_read(UART_DR) & 0xFF;

            size_t next_head = (rx_head + 1) & (RX_BUF_SIZE - 1);
            if (next_head != rx_tail) {
                rx_buf[rx_head] = c;
                rx_head = next_head;
            }
        }
    }

    /* TX pending */
    if (mis & (1 << 6)) {
        while (!tx_empty() && !(mmio_read(UART_FR) & (1 << 5))) {  /* !TXFF */
            mmio_write(UART_DR, tx_buf[tx_tail]);
            tx_tail = (tx_tail + 1) & (TX_BUF_SIZE - 1);
        }

        if (tx_empty()) {
            mmio_write(UART_IMSC, mmio_read(UART_IMSC) & ~(1 << 6));
        }
    }

    /* Clear exactly the interrupts that were masked-pending */
    mmio_write(UART_ICR, mis);
}

/** Sends a single character (CRLF handling, low-power wait if buffer full) */
void uart_putc(unsigned char c) {
    if (c == '\n') uart_putc('\r');

    /* Wait for space with low-power sleep */
    while (tx_full()) {
        asm volatile("wfi");
    }

    bool was_empty = tx_empty();

    tx_buf[tx_head] = c;
    tx_head = (tx_head + 1) & (TX_BUF_SIZE - 1);

    if (was_empty) {
        start_tx();
    }
}

/** Receives a single character (low-power wait if no data) */
unsigned char uart_getc(void) {
    while (rx_empty()) {
        asm volatile("wfi");
    }

    unsigned char c = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) & (RX_BUF_SIZE - 1);
    return c;
}