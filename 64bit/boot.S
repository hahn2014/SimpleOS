.section ".text.boot"

.global _start

_start:
	/* Read CPU ID (MPIDR_EL1) and keep only affine core ID (bits 0-1 on Pi, bits 0-3 on newer) /
	mrs     x1, mpidr_el1
	and     x1, x1, #0xFF
	cbz     x1, master_core          / Core 0 continues, others park */

park:
	wfe
	b       park

master_core:
	/* Set a safe stack pointer high in memory (256 MiB address – well above load address and safe in QEMU/default RAM) */
	mov     sp, #0x10000000
	/* Clear BSS */
	ldr     x1, =__bss_start
	ldr     x2, =__bss_end

clear_bss:
	cmp     x1, x2
	bge     clear_done
	str     xzr, [x1], #8
	b       clear_bss

clear_done:
	bl      kernel_main
	/* Should never return */
	b       parkå