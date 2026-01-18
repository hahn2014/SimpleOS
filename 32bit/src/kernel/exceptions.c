#include <common/stdio.h>

/* Helper to get readable mode name from CPSR mode bits */
static const char* mode_name(uint32_t mode) {
    switch (mode & 0x1F) {
        case 0x10: return "User";
        case 0x11: return "FIQ";
        case 0x12: return "IRQ";
        case 0x13: return "Supervisor";
        case 0x16: return "Monitor";
        case 0x17: return "Abort";
        case 0x1A: return "Hypervisor";
        case 0x1B: return "Undefined";
        case 0x1F: return "System";
        default:   return "Unknown";
    }
}

/* Common general register dump - called by all handlers */
static void dump_general(void) {
    uint32_t cpsr, lr;

    asm volatile("mrs %0, cpsr" : "=r"(cpsr));
    asm volatile("mov %0, lr" : "=r"(lr));

    printf("Current mode  : %s (0x%02X)\n", mode_name(cpsr), cpsr & 0x1F);
    printf("LR (return)   : 0x%08X\n", lr);
    printf("CPSR          : 0x%08X\n", cpsr);
}

void undefined_handler(void) {
    printf("\n=== KERNEL PANIC: Undefined Instruction ===\n");
    dump_general();
    /* No specific fault registers for UDF - LR_und points ~4 bytes before faulting instr */
    printf("Likely fault PC: 0x%08X\n", __builtin_return_address(0) - 4);
    printf("System halted.\n");
    while (1) asm volatile("wfi");
}

void svc_handler(void) {
    printf("\n=== KERNEL PANIC: Supervisor Call (SVC) ===\n");
    dump_general();
    /* SVC number could be extracted from instruction at LR-4, but omitted for simplicity */
    printf("System halted.\n");
    while (1) asm volatile("wfi");
}

void prefetch_abort_handler(void) {
    uint32_t ifsr, ifar;

    asm volatile("mrc p15, 0, %0, c5, c0, 1" : "=r"(ifsr));  // Instruction Fault Status
    asm volatile("mrc p15, 0, %0, c6, c0, 2" : "=r"(ifar));  // Instruction Fault Address

    printf("\n=== KERNEL PANIC: Prefetch Abort ===\n");
    dump_general();
    printf("Fault address : 0x%08X\n", ifar);
    printf("Fault status  : 0x%08X\n", ifsr);
    printf("System halted.\n");
    while (1) asm volatile("wfi");
}

void data_abort_handler(void) {
    uint32_t dfsr, dfar;

    asm volatile("mrc p15, 0, %0, c5, c0, 0" : "=r"(dfsr));  // Data Fault Status
    asm volatile("mrc p15, 0, %0, c6, c0, 0" : "=r"(dfar));  // Data Fault Address

    printf("\n=== KERNEL PANIC: Data Abort ===\n");
    dump_general();
    printf("Fault address : 0x%08X\n", dfar);
    printf("Fault status  : 0x%08X\n", dfsr);
    printf("System halted.\n");
    while (1) asm volatile("wfi");
}

void irq_handler(void) {
    printf("\n=== KERNEL PANIC: Unexpected IRQ ===\n");
    dump_general();
    printf("System halted.\n");
    while (1) asm volatile("wfi");
}

void fiq_handler(void) {
    printf("\n=== KERNEL PANIC: Unexpected FIQ ===\n");
    dump_general();
    printf("System halted.\n");
    while (1) asm volatile("wfi");
}