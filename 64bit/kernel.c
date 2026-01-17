#include "uart.h"
#include "types.h"

/* Minimal delay for flood visibility and backend flushing */
static void delay(uint32_t count) {
	while (count--) asm volatile ("nop");
}

void kernel_main(void) {
	uart_init();
	/* Flood mode: repeatedly print to force output visibility */
	while (1) {
		uart_puts("Hello, World! (flood mode for debug)\r\n");
		delay(1000000);  /* Adjustable pause – longer for slower scrolling, shorter for faster confirmation */
	}

	/* Once output confirmed, revert to normal echo loop by uncommenting below and removing flood:
	uart_puts("Hello, World!\r\n");
	while (1) {
	uart_putc(uart_getc());
	uart_puts("\r\n");
	}
	*/
}