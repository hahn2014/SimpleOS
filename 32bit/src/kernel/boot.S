.section ".text.boot"

.global _start

_start:
    mrc p15, #0, r1, c0, c0, #5
    and r1, r1, #3
    cmp r1, #0
    bne halt

    /* Copy vector table to 0x00000000 */
    ldr r0, =vector_table       /* source: address of our table */
    mov r1, #0x0000             /* destination: low vectors */
    ldmia r0!, {r2-r9}          /* load 8 words (32 bytes) into r2-r9 */
    stmia r1!, {r2-r9}          /* store them at 0x00000000 */

    mov sp, #0x8000

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
