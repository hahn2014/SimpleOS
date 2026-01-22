/************************************************************
 *                                                          *
 *             ~ SimpleOS - exceptions.c ~                  *
 *                                                          *
 *  Exception handlers - dump useful state and halt on      *
 *  fatal exceptions. IRQ handler forwards to timer.        *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 21 2026                          *
 *  ToDo: Recoverable page fault handling                   *
 ************************************************************/

#include <common/stdio.h>
#include <kernel/uart.h>
#include <kernel/timer.h>
#include <kernel/mmio.h>
#include <kernel/scheduler.h>

/* IRQ interrupt pointers */
#define IRQ_BASIC_PENDING   0x3F00B200UL
#define IRQ_PENDING1        0x3F00B204UL
#define IRQ_PENDING2        0x3F00B208UL

/* add extern for extended buffer */
extern uint32_t interrupt_context[16];

/* Forward declaration for UART handler (defined in uart.c) */
void uart_irq_handler(void);

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
    uint32_t basic    = mmio_read(IRQ_BASIC_PENDING);
    uint32_t pending1 = mmio_read(IRQ_PENDING1);
    uint32_t pending2 = mmio_read(IRQ_PENDING2);

    bool handled = false;

    if (pending1 & (1 << 1)) {          /* System Timer Match 1 */
        timer_handler();
        handled = true;
    }

    if (pending2 & (1 << 25)) {         /* PL011 UART0 (IRQ 57) */
        uart_irq_handler();
        handled = true;
    }

    if (!handled) {
        panic("Unhandled IRQ: basic=0x%08X pending1=0x%08X pending2=0x%08X",
              basic, pending1, pending2);
    }
}

void c_irq_handler(void) {
    /* Always service UART - safe/idempotent */
    uart_irq_handler();

    /* Service system timer if pending */
    uint32_t cs = mmio_read(TIMER_CS);
    if (cs & TIMER_CS_M1) {
        timer_handler();
        schedule();                 /* Preemptive tick */
        /* No return - schedule() restores new context and returns there */
    }

    /* If we reach here - unhandled source. For now silent
        (common on real hardware). Later add pending register check. */
    // panic("Unhandled IRQ source - TIMER_CS=0x%08X", cs);
}