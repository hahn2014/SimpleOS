#include <kernel/timer.h>
#include <kernel/uart.h>    /* For mmio_read / mmio_write */
#include <common/stdio.h>

volatile uint32_t system_ticks = 0;

void timer_init(void) {
    info("Initializing system timer");

    /* Clear any pending match 1 */
    mmio_write(TIMER_CS, TIMER_CS_M1);

    /* Set first compare ~10ms from now */
    uint32_t clo = mmio_read(TIMER_CLO);
    mmio_write(TIMER_C1, clo + 10000);  /* 1MHz counter → 10,000 = 10ms */
    debug("[timer.c] - System timer is calibrated to 1MHz (10ms)");

    /* Enable system timer match 1 IRQ in the interrupt controller */
    /* BCM2836 interrupt controller base: 0x3F00B000 */
    /* Enable IRQs register 1: offset 0x210, bit 1 = system timer 1 */
    mmio_write(0x3F00B210, mmio_read(0x3F00B210) | (1 << 1));
    info("System timer is running (~100 Hz tick)");
}

void timer_handler(void) {
    system_ticks++;

    /* Clear the match flag */
    mmio_write(TIMER_CS, TIMER_CS_M1);

    /* Schedule next tick ~10ms */
    uint32_t clo = mmio_read(TIMER_CLO);
    mmio_write(TIMER_C1, clo + 10000);
}

uint32_t get_uptime_ticks(void) {
    return system_ticks;
}

void delay_ms(uint32_t ms) {
    uint32_t target = system_ticks + (ms * 100);  /* ~100 ticks/sec */
    while (system_ticks < target) {
        asm volatile("wfi");
    }
}