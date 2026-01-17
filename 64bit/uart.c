#include "uart.h"
#include "types.h"

/* Peripheral base for Raspberry Pi 4 & 5 */
#define PBASE               0xFE000000ULL
/* PL011 UART registers */
#define UART_BASE           (PBASE + 0x201000ULL)
#define UART_DR             (UART_BASE + 0x00ULL)
#define UART_FR             (UART_BASE + 0x18ULL)
#define UART_IBRD           (UART_BASE + 0x24ULL)
#define UART_FBRD           (UART_BASE + 0x28ULL)
#define UART_LCR_H          (UART_BASE + 0x2CULL)
#define UART_CR             (UART_BASE + 0x30ULL)
#define UART_ICR            (UART_BASE + 0x44ULL)
/* GPIO registers (only for real hardware) */
#define GPIO_BASE           (PBASE + 0x200000ULL)
#define GPFSEL1             (GPIO_BASE + 0x04ULL)
#define GPPUPPDN0           (GPIO_BASE + 0xE4ULL)

static inline void mmio_write(uint64_t reg, uint32_t data) {
	*(volatile uint32_t *)reg = data;
}

static inline uint32_t mmio_read(uint64_t reg) {
	return *(volatile uint32_t *)reg;
}

/* Busy-wait delay - tuned for visibility in QEMU during debug flooding */
static void delay(uint32_t count) {
	while (count--) asm volatile ("nop");
}

void uart_init(void) {
	mmio_write(UART_CR, 0);  /* Disable UART */

	#ifdef REAL_HARDWARE
		/* Real hardware: GPIO 14/15 Alt0 for PL011 */
		uint32_t r = mmio_read(GPFSEL1);
		r &= ~((7 << 12) | (7 << 15));
		r |= (4 << 12) | (4 << 15);
		mmio_write(GPFSEL1, r);
		/* Disable pull */
		r = mmio_read(GPPUPPDN0);
		r &= ~(3 << 28);
		r &= ~(3 << 30);
		mmio_write(GPPUPPDN0, r);
	#endif

	mmio_write(UART_IBRD, 26);
	mmio_write(UART_FBRD, 3);
	mmio_write(UART_LCR_H, 0x70);  /* 8n1, FIFO enable */
	mmio_write(UART_ICR, 0x7FF);   /* Clear interrupts */
	delay(20000);  /* Stabilisation */
	mmio_write(UART_CR, 0x301);   /* Enable UART, TX, RX */
}

void uart_putc(unsigned char c) {
	mmio_write(UART_DR, c);  /* Direct write - optimal for QEMU raspi4b PL011 emulation */
}

unsigned char uart_getc(void) {
	while (mmio_read(UART_FR) & (1 << 4)) {}  /* RXFE */
	return (unsigned char)mmio_read(UART_DR);
}

void uart_puts(const char *str) {
	while (*str) {
		if (*str == '\n') {
			uart_putc('\r');
		}
		uart_putc(*str++);   /* Fixed: dereference before post-increment */
		delay(100);          /* Small per-char delay for reliable flushing in flood/debug mode */
	}
}