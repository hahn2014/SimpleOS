.section .text
.align 5   /* 32-byte alignment */

.global vector_table

vector_table:
    /* Direct branches - keep as-is */
    b   reset_handler           /* Reset - unused, but branch to safe hang */
    b   undefined_handler       /* Undefined Instruction - direct to C handler */
    b   svc_handler             /* Supervisor Call */
    b   prefetch_abort_handler  /* Prefetch Abort */
    b   data_abort_handler      /* Data Abort */
    nop                         /* Reserved - must be a valid instruction (nop is fine) */
    b   irq_handler             /* IRQ */
    b   fiq_handler             /* FIQ */

reset_handler:
    b   hang                    /* Safe fallback */

hang:
    wfi
    b   hang