/************************************************************
 *                                                          *
 *               ~ SimpleOS - shell.c ~                     *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Simple interactive command shell. Provides basic        *
 *  system control and debugging commands.                  *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add command history and line editing              *
 ************************************************************/

#include <common/stdio.h>
#include <common/stdlib.h>
#include <kernel/mem.h>
#include <kernel/timer.h>

/** Prints the list of available shell commands */
static void cmd_help(void) {
    printf("Available commands:\n"
           "  help       - Show this help\n"
           "  meminfo    - Display memory statistics\n"
           "  uptime     - Show system uptime\n"
           "  shutdown   - Halt the system safely\n"
           "  reboot     - Restart the kernel\n"
           "  test_undef - Trigger Undefined Instruction exception\n"
           "  test_abort - Trigger Data Abort exception\n");
}

/** Displays current page allocator statistics */
static void cmd_meminfo(void) {
    info("Total pages : %u", get_total_pages());
    info("Free pages  : %u", get_free_pages());
}

/** Halts the CPU in a low-power state (system shutdown) */
static void cmd_shutdown(void) {
    warning("Shutting down SimpleOS...\nSystem halted.\n");
    while (1) {
        asm volatile("wfi");
    }
}

/** Performs a soft reset by disabling MMU/caches and jumping to _start */
static void cmd_reboot(void) {
    printf("Rebooting system...\n");

    /* Disable global interrupts */
    asm volatile (
        "mrs r0, cpsr\n"
        "orr r0, r0, #0xC0\n"       /* Set I and F bits */
        "msr cpsr_c, r0\n"
        ::: "r0"
    );

    /* Disable system timer IRQ */
    mmio_write(0x3F00B210, mmio_read(0x3F00B210) & ~(1 << 1));

    /* Clear pending match */
    mmio_write(TIMER_CS, TIMER_CS_M1);

    /* Barriers */
    asm volatile("dsb"); asm volatile("isb");

    /* Safely disable MMU, caches and branch predictor before reset */
    uint32_t sctlr;
    asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r"(sctlr));
    sctlr &= ~((1 << 12) | (1 << 2) | (1 << 11) | (1 << 0)); /* Clear I-cache, D-cache, BP, MMU */
    asm volatile("mcr p15, 0, %0, c1, c0, 0" :: "r"(sctlr));
    asm volatile("isb");

    /* Invalidate unified TLB */
    asm volatile("mcr p15, 0, %0, c8, c7, 0" :: "r"(0));
    asm volatile("dsb"); asm volatile("isb");

    /* Jump back to kernel entry - soft reset */
    asm volatile("ldr pc, =_start");
    while (1); /* Never reached */
}

/** Shows system uptime in ticks and approximate seconds */
static void cmd_uptime(void) {
    uint32_t ticks = get_uptime_ticks();
    printf("Uptime: %u ticks (~%u.%02u seconds)\n", ticks, ticks / 100, ticks % 100);
}

/** Triggers an Undefined Instruction exception for testing */
static void cmd_test_undef(void) {
    warning("Triggering Undefined Instruction exception...");
    asm volatile("udf #0");
}

/** Triggers a Data Abort exception for testing */
static void cmd_test_abort(void) {
    warning("Triggering Data Abort exception...");
    *(volatile uint32_t *)0xDEADBEEF = 0xBAD;
}

/** Entry point for the interactive shell - prints banner and enters command loop */
void shell_run(void) {
    printf("\n\nSimpleOS v0.03-alpha(arm32) - Interactive Shell\n\n\n");
    cmd_help();

    while (1) {
        char buf[128];

        bzero(buf, sizeof(buf));       /* Clear to prevent leftover garbage */

        printf("\n> ");
        gets(buf, sizeof(buf));

        if (buf[0] == '\0') continue;  /* Empty line */

        if (strcmp(buf, "help") == 0)          cmd_help();
        else if (strcmp(buf, "meminfo") == 0)  cmd_meminfo();
        else if (strcmp(buf, "uptime") == 0)   cmd_uptime();
        else if (strcmp(buf, "shutdown") == 0) cmd_shutdown();
        else if (strcmp(buf, "reboot") == 0)   cmd_reboot();
        else if (strcmp(buf, "test_undef") == 0) cmd_test_undef();
        else if (strcmp(buf, "test_abort") == 0) cmd_test_abort();
        else printf("Unknown command: %s\n", buf);
    }
}