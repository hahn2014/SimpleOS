/************************************************************
 *                                                          *
 *               ~ SimpleOS - kernel.c ~                    *
 *                  version 0.06-alpha                      *
 *                                                          *
 * Kernel entry point and top-level initialisation sequence *
 *  Called from boot.S after BSS clear.                     *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 21 2026                          *
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
#include <kernel/scheduler.h>
#include <common/stdio.h>
#include <common/stdlib.h>


/* Add test task functions */
static void test_task_a(void) {
    while (1) {
        info(">>> Running in test_task_a");
        delay_ms(2000);
    }
}

static void test_task_b(void) {
    while (1) {
        info(">>> Running in test_task_b");
        delay_ms(3000);
    }
}

/** Main kernel entry point - performs all early initialisation before shell */
void kernel_main(uint32_t atags) {

    uart_init();                /* Early serial for debug output */
    mem_init((atag_t *)atags);  /* Physical memory manager + heap */
    mmu_init();                 /* Identity mapping + enable MMU/caches */
    timer_init();               /* System timer for uptime & delays */
    test_mem();                 /* Quick sanity check of alloc/free */
    scheduler_init();           /* Round-Robin Scheduler */

    // scheduler tests (schedule memory is not fully functional. commenting out for usability)
    // task_create(test_task_a, "task_a");
    // task_create(test_task_b, "task_b");

    // info("Test tasks created - scheduler active");

    /* Enable interrupts now that critical init is complete */
    asm volatile (
        "mrs r0, cpsr\n"
        "bic r0, r0, #0x80\n"   /* Clear I bit */
        "msr cpsr_c, r0\n"
        ::: "r0"
    );

    shell_run();                /* Drop to interactive shell - never returns */
}