ENTRY(_start)

SECTIONS {
	. = 0x80000;                /* Raspberry Pi firmware loads kernel8.img at 0x80000 */
	.text : {
		KEEP(*(.text.boot))
        *(.text)
	}

	.rodata : {
		*(.rodata)
	}

	.data : {
		*(.data)
	}

	.bss : {
		__bss_start = .;
		*(COMMON)
		*(.bss)
		__bss_end = .;
	}
}