/************************************************************
 *                                                          *
 *               ~ SimpleOS - timer.c ~                     *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  System timer (1 MHz free-running counter) driver.       *
 *  Provides ~100 Hz tick for uptime and simple delays.     *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Higher-resolution scheduling primitives           *
 ************************************************************/

#include <kernel/timer.h>
#include <kernel/uart.h>
#include <common/stdio.h>

volatile uint32_t system_ticks = 0;

void timer_init(void) {
    info("Initializing system timer");

    mmio_write(TIMER_CS, TIMER_CS_M1);               /* Clear any pending match */
    uint32_t clo = mmio_read(TIMER_CLO);
    mmio_write(TIMER_C1, clo + 50000);               /* First tick delayed for safety */

    /* Enable timer channel 1 interrupt in GPU interrupt controller */
    mmio_write(0x3F00B210, mmio_read(0x3F00B210) | (1 << 1));

    info("System timer armed - first tick in ~50 ms");
}

void timer_handler(void) {
    system_ticks++;

    mmio_write(TIMER_CS, TIMER_CS_M1);               /* Clear match flag */
    uint32_t clo = mmio_read(TIMER_CLO);
    mmio_write(TIMER_C1, clo + 10000);               /* Next tick in 10 ms (100 Hz) */
}

uint32_t get_uptime_ticks(void) {
    return system_ticks;
}

/* Busy-wait delay using tick counter - low power via WFI */
void delay_ms(uint32_t ms) {
    uint32_t target = system_ticks + ms;             /* 100 ticks ≈ 1 second */
    while (system_ticks < target)
        asm volatile("wfi");
}