/************************************************************
 *                                                          *
 *                  ~ SimpleOS - kernel.c ~                 *
 *                                                          *
 *  SimpleOS Kernel was written by github.com/jsandler18    *
 *      and Bryce Hahn github.com/hahn2014                  *
 *  Last Modified: July 16 2020                             *
 *  ToDo: Get a bootable kernel setup                       *
 ************************************************************/
 #include <stddef.h>
 #include <stdint.h>
 #include <kernel/uart.h>
 #include <kernel/atag.h>
 #include <kernel/mem.h>
 #include <common/stdio.h>
 #include <common/stdlib.h>

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
    char buf[256];
    //init registers as empty
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init();
    puts("Initializing Memory Module\n");
    mem_init((atag_t*)atags);

    puts("Hello, World!\n");
    while (1) {
        gets(buf, 256);
        puts("> ");
        puts(buf);
        putc('\n');
        bzero(buf, 256);
    }
}
