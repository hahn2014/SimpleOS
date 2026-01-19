.section ".text.boot"

.global _start

_start:
    /* Only core 0 boots - park others */
    mrc p15, #0, r1, c0, c0, #5
    and r1, r1, #3
    cmp r1, #0
    bne halt

    /* Set VBAR to our vector table (no low-memory copy needed) */
    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0   /* Write VBAR */

    /* High, safe stack - 64MB into RAM (well above kernel) */
    mov sp, #0x04000000         /* SVC mode initial stack */

    /* Banked exception mode stacks - share the same high area */
    mov r0, #0x04000000

    cps #0x1B                   /* Undefined */
    mov sp, r0

    cps #0x17                   /* Abort */
    mov sp, r0

    cps #0x12                   /* IRQ */
    mov sp, r0

    cps #0x11                   /* FIQ */
    mov sp, r0

    cps #0x13                   /* Back to SVC */

    mrs r0, cpsr
    bic r0, r0, #0x80  /* Clear I bit */
    msr cpsr_c, r0

    ldr r4, =__bss_start
    ldr r9, =__bss_end
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    b       2f

1:
    stmia r4!, {r5-r8}

2:
    cmp r4, r9
    blo 1b

    ldr r3, =kernel_main
    blx r3

halt:
    wfe
    b halt
