/************************************************************
 *                                                          *
 *                ~ SimpleOS - linker.ld ~                  *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Linker script for the bare-metal kernel. Places code    *
 *  at 0x8000 (standard Raspberry Pi kernel load address).  *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add separate sections for device tree / initrd    *
 ************************************************************/

ENTRY(_start)

SECTIONS
{
    . = 0x8000;                     /* Kernel load address */
    __start = .;

    __text_start = .;
    .text : {
        KEEP(*(.text.boot))
        *(.text)
    }
    . = ALIGN(4096);
    __text_end = .;

    __rodata_start = .;
    .rodata : { *(.rodata) }
    . = ALIGN(4096);
    __rodata_end = .;

    __data_start = .;
    .data : { *(.data) }
    . = ALIGN(4096);
    __data_end = .;

    __bss_start = .;
    .bss : {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096);
    __bss_end = .;
    __end = .;
}