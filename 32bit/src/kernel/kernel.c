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
    (void)buf;
    //init registers as empty
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init();
    puts("\nSimpleOS v0.01-alpha\n\n\n");
    info("Initializing Memory Module\n");
    mem_init((atag_t*)atags);





    info("Testing memory allocation...");
    void* p1 = alloc_page();
    void* h1 = kmalloc(128);
    puts("Page allocated at 0x"); puts(itoa((uint32_t)p1)); puts("\n");
    puts("Heap allocated at 0x"); puts(itoa((uint32_t)h1)); puts("\n");
    free_page(p1);
    kfree(h1);
    info("Memory test complete\n");


    puts("\nType 'test_undef' to trigger Undefined Instruction exception\n");
    puts("Type 'q' to abort SimpleOS and quit HW emulation\n");
    puts("Type 'test_abort' to trigger Data Abort exception\n");
    puts("Type anything else to echo\n");

    while (1) {
        char buf[64];
        puts("> ");
        gets(buf, sizeof(buf));

        if (strcmp(buf, "q") == 0) { // Kernel abort
            break;
        } else if (strcmp(buf, "test_undef") == 0) {
            puts("Triggering Undefined Instruction...\n");
            asm volatile("udf #0");
        } else if (strcmp(buf, "test_abort") == 0) {
            puts("Triggering Data Abort...\n");
            *(volatile uint32_t*)0xDEADBEEF = 0xBAD;
        } else {
            puts("Echo: ");
            puts(buf);
            puts("\n");
        }
    }
}
