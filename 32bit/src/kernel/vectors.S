.section .text
.align 11                   /* Align to 2KB - not strictly needed for low vectors but harmless */

.global vector_table

vector_table:
    b reset_handler         /* Reset - not used (kernel loaded directly) */
    b undefined_handler
    b svc_handler
    b prefetch_abort_handler
    b data_abort_handler
    nop                     /* Reserved */
    b irq_handler
    b fiq_handler

reset_handler:
    b hang                  /* Fallback if somehow taken */

undefined_handler:
    b undefined_handler      /* Direct loop to self - C handler will run via attribute */

svc_handler:
    b svc_handler

prefetch_abort_handler:
    b prefetch_abort_handler

data_abort_handler:
    b data_abort_handler

irq_handler:
    b irq_handler

fiq_handler:
    b fiq_handler

hang:
    wfi
    b hang