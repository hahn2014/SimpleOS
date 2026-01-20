/************************************************************
 *                                                          *
 *               ~ SimpleOS - kernel.c ~                    *
 *                     version 0.04-alpha                   *
 *                                                          *
 * Kernel entry point and top-level initialisation sequence *
 *  Called from boot.S after BSS clear.                     *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Implement proper multi-core startup               *
 ************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <kernel/uart.h>
#include <kernel/atag.h>
#include <kernel/mem.h>
#include <kernel/shell.h>
#include <kernel/mmu.h>
#include <kernel/timer.h>
#include <common/stdio.h>
#include <common/stdlib.h>

/** Main kernel entry point - performs all early initialisation before shell */
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
    (void)r0;
    (void)r1;

    uart_init();               /* Early serial for debug output */
    mem_init((atag_t *)atags); /* Physical memory manager + heap */
    mmu_init();                /* Identity mapping + enable MMU/caches */
    timer_init();              /* System timer for uptime & delays */
    test_mem();                /* Quick sanity check of alloc/free */

    /* Enable interrupts now that critical init is complete */
    asm volatile (
        "mrs r0, cpsr\n"
        "bic r0, r0, #0x80\n"   /* Clear I bit */
        "msr cpsr_c, r0\n"
        ::: "r0"
    );

    shell_run();               /* Drop to interactive shell - never returns */
}