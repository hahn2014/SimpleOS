/************************************************************
 *                                                          *
 *              ~ SimpleOS - vectors.S ~                    *
 *                                                          *
 * Full preemptive context switch with private SVC stacks.  *
 *  Mode switch to save/restore banked r13/r14_svc. Fixed   *
 *  offsets — zero warnings, predictable, low overhead.     *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 21 2026                          *
 *  ToDo: Add full context save/restore for all exceptions  *
 ************************************************************/

.section .text
.balign 32

.global vector_table

vector_table:
    b   reset_handler
    b   undefined_handler
    b   svc_handler
    b   prefetch_abort_handler
    b   data_abort_handler
    nop
    b   irq_stub
    b   fiq_handler

reset_handler:
    b   hang

hang:
    wfi
    b   hang

irq_stub:
    ldr     r1, =interrupt_context

    /* Save r0-r11 */
    stmia   r1, {r0-r11}

    /* Save r12, IRQ_sp, LR_irq, SPSR */
    str     r12, [r1, #48]
    str     sp,  [r1, #52]
    str     lr,  [r1, #56]
    mrs     r0, spsr
    str     r0,  [r1, #60]

    bl      c_irq_handler   /* timer_handler → schedule() */

    /* Restore */
    ldr     r1, =interrupt_context

    ldmia   r1, {r0-r11}

    ldr     r12, [r1, #48]
    ldr     sp,  [r1, #52]   /* Private IRQ stack */
    ldr     lr,  [r1, #56]
    ldr     r0,  [r1, #60]
    msr     spsr_cxsf, r0

    subs    pc, lr, #4

.global interrupt_context
.section .bss
interrupt_context:
    .space 64               /* 16 × 4 bytes */