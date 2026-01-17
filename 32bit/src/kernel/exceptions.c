#include <common/stdio.h>

void __attribute__((interrupt("UNDEF"))) undefined_handler(void) {
    panic("Undefined Instruction exception");
}

void __attribute__((interrupt("SWI"))) svc_handler(void) {
    panic("Supervisor Call (SVC) exception");
}

void __attribute__((interrupt("ABRT"))) prefetch_abort_handler(void) {
    panic("Prefetch Abort exception");
}

void __attribute__((interrupt("ABRT"))) data_abort_handler(void) {
    panic("Data Abort exception");
}

void __attribute__((interrupt("IRQ"))) irq_handler(void) {
    panic("Unexpected IRQ");
}

void __attribute__((interrupt("FIQ"))) fiq_handler(void) {
    panic("Unexpected FIQ");
}