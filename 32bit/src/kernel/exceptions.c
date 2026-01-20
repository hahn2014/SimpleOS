/************************************************************
 *                                                          *
 *             ~ SimpleOS - exceptions.c ~                  *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Exception handlers - dump useful state and halt on      *
 *  fatal exceptions. IRQ handler forwards to timer.        *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Recoverable page fault handling                   *
 ************************************************************/

#include <common/stdio.h>
#include <kernel/uart.h>
#include <kernel/timer.h>

static const char *mode_name(uint32_t mode) {
    switch (mode & 0x1F) {
        case 0x10: return "User";
        case 0x11: return "FIQ";
        case 0x12: return "IRQ";
        case 0x13: return "Supervisor";
        case 0x16: return "Monitor";
        case 0x17: return "Abort";
        case 0x1B: return "Undefined";
        case 0x1F: return "System";
        default:   return "Unknown";
    }
}

/** Dumps basic register state (mode, LR, CPSR) for exception debugging */
static void dump_general(void) {
    uint32_t cpsr, lr;
    asm volatile("mrs %0, cpsr" : "=r"(cpsr));
    asm volatile("mov %0, lr"   : "=r"(lr));
    printf("Mode : %s (0x%02X)\n", mode_name(cpsr), cpsr & 0x1F);
    printf("LR   : 0x%08X\n", lr);
    printf("CPSR : 0x%08X\n", cpsr);
}

/** Handler for Undefined Instruction exceptions - prints info and halts */
void undefined_handler(void) {
    printf("\n=== UNDEFINED INSTRUCTION ===\n");
    dump_general();
    while (1) {
        asm volatile("wfi");
    }
}

/** Handler for SVC exceptions - prints info and halts */
void svc_handler(void) {
    printf("\n=== SVC ===\n");
    dump_general();
    while (1) {
        asm volatile("wfi");
    }
}

/** Handler for Prefetch Abort exceptions - prints info and halts */
void prefetch_abort_handler(void) {
    printf("\n=== PREFETCH ABORT ===\n");
    dump_general();
    while (1) {
        asm volatile("wfi");
    }
}

/** Handler for Data Abort exceptions - prints fault address/status and halts */
void data_abort_handler(void) {
    uint32_t dfsr, dfar;
    asm volatile("mrc p15, 0, %0, c5, c0, 0" : "=r"(dfsr));
    asm volatile("mrc p15, 0, %0, c6, c0, 0" : "=r"(dfar));
    printf("\n=== DATA ABORT ===\n");
    dump_general();
    printf("DFAR : 0x%08X\n", dfar);
    printf("DFSR : 0x%08X\n", dfsr);
    while (1) {
        asm volatile("wfi");
    }
}

/** Handler for unexpected FIQ exceptions - prints info and halts */
void fiq_handler(void) {
    printf("\n=== UNEXPECTED FIQ ===\n");
    dump_general();
    while (1) {
        asm volatile("wfi");
    }
}

/** Primary IRQ handler - currently only the system timer is expected */
void irq_handler(void) {
    uint32_t cs = mmio_read(TIMER_CS);
    if (cs & TIMER_CS_M1) {
        timer_handler();
        return;
    }
    panic("Unhandled IRQ - TIMER_CS = 0x%08X", cs);
}