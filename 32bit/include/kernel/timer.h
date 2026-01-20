/************************************************************
 *                                                          *
 *                 ~ SimpleOS - timer.h ~                   *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Declarations for the BCM283x system timer driver.       *
 *  Provides ~100 Hz tick and simple delay functions.       *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Higher-resolution scheduling primitives           *
 ************************************************************/

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/* BCM2836/2837 System Timer physical base address */
#define TIMER_BASE      0x3F003000UL

/* System Timer registers (relative to TIMER_BASE) */
#define TIMER_CS        (TIMER_BASE + 0x00UL)  /* Control/Status */
#define TIMER_CLO       (TIMER_BASE + 0x04UL)  /* Counter Lower 32-bit */
#define TIMER_C1        (TIMER_BASE + 0x10UL)  /* Compare channel 1 */
#define TIMER_CS_M1     (1UL << 1)             /* Match flag for channel 1 */

/** Initialises the system timer and arms the first interrupt (~50 ms delay) */
void timer_init(void);

/** Called from IRQ handler on each tick - increments uptime and arms next tick (10 ms) */
void timer_handler(void);

/** Returns the current system uptime in ticks (100 Hz) */
uint32_t get_uptime_ticks(void);

/** Busy-wait delay in milliseconds using the tick counter (low-power via WFI) */
void delay_ms(uint32_t ms);

#endif /* TIMER_H */