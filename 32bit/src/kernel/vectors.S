/************************************************************
 *                                                          *
 *              ~ SimpleOS - vectors.S ~                    *
 *                                                          *
 *  Exception vector table and low-level stubs. Required    *
 *  to be 32-byte aligned. Direct branches for most         *
 *  exceptions (they panic). Dedicated stub for IRQ to      *
 *  correctly save/restore state.                           *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add full context save/restore for all exceptions  *
 ************************************************************/

.section .text
.align 5                               /* 32-byte alignment required by Cortex-A7 */

.global vector_table

vector_table:
    b   reset_handler                  /* Reset */
    b   undefined_handler              /* Undefined Instruction */
    b   svc_handler                    /* Supervisor Call */
    b   prefetch_abort_handler         /* Prefetch Abort */
    b   data_abort_handler             /* Data Abort */
    nop                                /* Reserved */
    b   irq_stub                       /* IRQ - proper stub */
    b   fiq_handler                    /* FIQ */

reset_handler:
    b   hang                           /* Safety - should never return here */

hang:
    wfi
    b   hang

/* Proper IRQ entry - saves AAPCS clobbered registers, calls C handler, correct return */
irq_stub:
    push    {r0-r3, ip, lr}             /* Save volatiles + link register */
    bl      irq_handler                /* C handler (exceptions.c) */
    pop     {r0-r3, ip, lr}
    subs    pc, lr, #4                 /* Return adjusting for pipeline */