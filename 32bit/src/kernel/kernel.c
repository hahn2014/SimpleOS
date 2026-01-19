/************************************************************
 *                                                          *
 *                  ~ SimpleOS - kernel.c ~                 *
 *                     version 0.03-alpha                   *
 *                                                          *
 *  SimpleOS Kernel was written by Bryce Hahn open source   *
 *      at github.com/hahn2014. 32bit core was inspired by  *
 *      github.com/jsandler18                               *
 *                                                          *
 *  License: MIT
 *  Last Modified: January 19 2026                          *
 *  ToDo: Memory leak fixes and L1 Pagetable                *
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

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
    //init registers as empty
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init(); // initialize serial comm
    mem_init((atag_t*)atags); // initialize memory logic
    mmu_init(); // startup MMU to initialize L1 pagetable
    // timer_init(); // initialize system timer
    test_mem(); // memory test for functionality
    shell_run(); // Mini Shell

    /* Enable global interrupts - safe now that all drivers/MMIO are initialized */
    asm volatile(
        "mrs r0, cpsr\n"
        "bic r0, r0, #0x80\n"   /* Clear I bit - enable IRQ */
        "msr cpsr_c, r0\n"
        ::: "r0"
    );
}