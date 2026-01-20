/************************************************************
 *                                                          *
 *                ~ SimpleOS - mmio.h ~                     *
 *                                                          *
 *  Inline helpers for safe memory-mapped I/O on BCM2836/7. *
 *  Volatile accesses + compiler barrier to prevent         *
 *  reordering. Minimal overhead, fully inlined.            *
 *                                                          *
 *  License: MIT                                            *
 *  Rationale: Volatile prevents read/write elimination or  *
 *  caching by compiler; "memory" clobber ensures ordering  *
 *  across multiple MMIO operations. DSB not strictly       *
 *  needed here (peripherals are device/strongly-ordered    *
 *  when properly mapped), but can be added later if        *
 *  issues arise.                                           *
 ************************************************************/

#ifndef _KERNEL_MMIO_H
#define _KERNEL_MMIO_H

#include <stdint.h>

/* Write 32-bit value to peripheral register */
static inline void mmio_write(uint32_t reg, uint32_t data) {
    /* Compiler barrier first (prevents reordering before write) */
    asm volatile("" ::: "memory");
    *(volatile uint32_t *)reg = data;
}

/* Read 32-bit value from peripheral register */
static inline uint32_t mmio_read(uint32_t reg) {
    uint32_t val = *(volatile uint32_t *)reg;
    /* Compiler barrier after (prevents reordering of subsequent code) */
    asm volatile("" ::: "memory");
    return val;
}

#endif /* _KERNEL_MMIO_H */