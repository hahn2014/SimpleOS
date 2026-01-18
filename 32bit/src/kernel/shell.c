#include <common/stdio.h>
#include <kernel/mem.h>
#include <common/stdlib.h>

static void cmd_help(void) {
    printf("Available commands:\n"
           "  help       - Show this help\n"
           "  meminfo    - Display memory statistics\n"
           "  shutdown   - Halt the system safely\n"
           "  test_undef - Trigger Undefined Instruction exception\n"
           "  test_abort - Trigger Data Abort exception\n");
}

static void cmd_meminfo(void) {
    printf("Total pages : %p\n", get_total_pages());
    printf("Free pages  : %p\n", get_free_pages());
}

static void cmd_shutdown(void) {
    printf("\nShutting down SimpleOS...\nSystem halted.\n\n");
    /* Safe infinite wait - low power, no further execution */
    while (1) {
        asm volatile("wfi");
    }
}

static void cmd_test_undef(void) {
    puts("Triggering Undefined Instruction exception...\n");
    asm volatile("udf #0");
}

static void cmd_test_abort(void) {
    puts("Triggering Data Abort exception...\n");
    *(volatile uint32_t*)0xDEADBEEF = 0xBAD;
}

void shell_run(void) {
    printf("\nSimpleOS v0.02-alpha(arm32) - Interactive Shell\n\n");
    cmd_help();

    while (1) {
        char buf[128];

        puts("\n> ");
        gets(buf, sizeof(buf));

        /* Skip empty lines */
        if (buf[0] == '\0') {
            continue;
        }

        if (strcmp(buf, "help") == 0) {
            cmd_help();
        } else if (strcmp(buf, "meminfo") == 0) {
            cmd_meminfo();
        } else if (strcmp(buf, "shutdown") == 0) {
            cmd_shutdown();
        } else if (strcmp(buf, "test_undef") == 0) {
            cmd_test_undef();
        } else if (strcmp(buf, "test_abort") == 0) {
            cmd_test_abort();
        } else {
            puts("Unknown command: ");
            puts(buf);
            puts("\n");
        }
    }
}