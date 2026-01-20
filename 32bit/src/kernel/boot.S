/************************************************************
 *                                                          *
 *                ~ SimpleOS - boot.S ~                     *
 *                                                          *
 *  Early boot code - executed directly by the Raspberry Pi *
 *  firmware with MMU off. Sets up initial stack, clears    *
 *  BSS, parks secondary cores, sets VBAR, and jumps to     *
 *  kernel_main.                                            *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add secondary core bring-up support               *
 ************************************************************/

.section ".text.boot"

.global _start

_start:
    /* Only core 0 continues - park others in low-power loop */
    mrc p15, #0, r1, c0, c0, #5    /* Read MPIDR (Affinity register) */
    and r1, r1, #3                 /* Extract CPU ID (bits 0-1) */
    cmp r1, #0
    bne halt                      /* Non-zero cores wait forever */

    /* Ensure IRQs are disabled at start */
    mrs r0, cpsr
    orr r0, r0, #0x80              /* Set I bit */
    msr cpsr_c, r0

    /* Set exception vector base address register */
    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0     /* VBAR = vector_table */

    /* Initial SVC mode stack - high in RAM to avoid kernel image */
    mov sp, #0x04000000           /* 64 MiB - safe above typical kernel load */

    /* Set up banked stacks for exception modes (shared high area) */
    mov r0, #0x04000000

    cps #0x1B                     /* Undefined mode */
    mov sp, r0

    cps #0x17                     /* Abort mode */
    mov sp, r0

    cps #0x12                     /* IRQ mode */
    mov sp, r0

    cps #0x11                     /* FIQ mode */
    mov sp, r0

    cps #0x13                     /* Back to SVC mode */

    /* Clear BSS section */
    ldr r4, =__bss_start
    ldr r9, =__bss_end
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    b       2f

1:  stmia r4!, {r5-r8}             /* Store 4 zeros */

2:  cmp r4, r9
    blo 1b

    /* Jump to C entry point */
    ldr r3, =kernel_main
    blx r3

halt:
    wfe                           /* Low-power wait */
    b halt