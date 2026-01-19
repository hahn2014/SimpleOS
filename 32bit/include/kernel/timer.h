#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/* BCM2836/2837 System Timer registers */
#define TIMER_BASE      0x3F003000UL
#define TIMER_CS        (TIMER_BASE + 0x00UL)  /* Control/Status */
#define TIMER_CLO       (TIMER_BASE + 0x04UL)  /* Counter Lower */
#define TIMER_C1        (TIMER_BASE + 0x10UL)  /* Compare 1 */

#define TIMER_CS_M1     (1UL << 1)  /* Match detect for timer 1 */

/* Public API */
void timer_init(void);
void timer_handler(void);
uint32_t get_uptime_ticks(void);
void delay_ms(uint32_t ms);

#endif