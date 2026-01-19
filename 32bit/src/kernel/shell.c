#include <common/stdio.h>
#include <common/stdlib.h>
#include <kernel/mem.h>
#include <kernel/timer.h>

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

static void cmd_meminfo(void) {
    info("Total pages : %p", get_total_pages());
    info("Free pages  : %p", get_free_pages());
}

static void cmd_shutdown(void) {
    warning("Shutting down SimpleOS...\nSystem halted.\n");
    /* Safe infinite wait - low power, no further execution */
    while (1) {
        asm volatile("wfi");
    }
}

static void cmd_reboot(void) {
    printf("Rebooting system...\n");

    /* Disable global IRQs to prevent pending interrupts during restart */
    asm volatile(
        "mrs r0, cpsr\n"
        "orr r0, r0, #0x80\n"   /* Set I bit */
        "msr cpsr_c, r0\n"
        ::: "r0"
    );

    /* Disable system timer IRQ in controller */
    mmio_write(0x3F00B210, mmio_read(0x3F00B210) & ~(1 << 1));

    /* Clear any pending timer match */
    mmio_write(TIMER_CS, TIMER_CS_M1);

    /* Small delay to ensure pending writes complete (conservative) */
    for (volatile int i = 0; i < 1000; i++) {}

    /* Restart kernel from entry point */
    asm volatile(
        "ldr r0, =_start\n"
        "bx r0\n"
        :: "r"(0) : "r0"
    );

    /* Unreachable */
    while (1) asm volatile("wfi");
}

static void cmd_uptime(void) {
    uint32_t ticks = get_uptime_ticks();
    printf("Uptime: %u ticks (~%u.%02u seconds)\n", ticks, ticks / 100, ticks % 100);
}

static void cmd_test_undef(void) {
    warning("Triggering Undefined Instruction exception...");
    asm volatile("udf #0");
}

static void cmd_test_abort(void) {
    warning("Triggering Data Abort exception...");
    *(volatile uint32_t*)0xDEADBEEF = 0xBAD;
}

void shell_run(void) {
    printf("\n\nSimpleOS v0.03-alpha(arm32) - Interactive Shell\n\n\n");
    cmd_help();

    while (1) {
        char buf[128];

        /* Zero buffer for safety - prevents garbage from previous frames triggering commands */
        bzero(buf, sizeof(buf));

        printf("\n> ");
        gets(buf, sizeof(buf));

        /* Skip empty lines */
        if (buf[0] == '\0') {
            continue;
        }

        /* Optional debug - remove once stable */
        // printf("Received command: '%s'\n", buf);

        if (strcmp(buf, "help") == 0) {
            cmd_help();
        } else if (strcmp(buf, "meminfo") == 0) {
            cmd_meminfo();
        } else if (strcmp(buf, "uptime") == 0) {
            cmd_uptime();
        } else if (strcmp(buf, "shutdown") == 0) {
            cmd_shutdown();
        } else if (strcmp(buf, "reboot") == 0) {
            cmd_reboot();
        } else if (strcmp(buf, "test_undef") == 0) {
            cmd_test_undef();
        } else if (strcmp(buf, "test_abort") == 0) {
            cmd_test_abort();
        } else {
            printf("Unknown command: %s\n", buf);
        }
    }
}