/************************************************************
 *                                                          *
 *                  ~ SimpleOS - kernel.c ~                 *
 *                                                          *
 *  SimpleOS Kernel was written by github.com/jsandler18    *
 *      and Bryce Hahn github.com/hahn2014                  *
 *  Last Modified: July 16 2020                             *
 *  ToDo: Mini-shell implementation                         *
 ************************************************************/
 #include <stddef.h>
 #include <stdint.h>
 #include <kernel/uart.h>
 #include <kernel/atag.h>
 #include <kernel/mem.h>
 #include <common/stdio.h>
 #include <common/stdlib.h>
 #include <kernel/shell.h>

void test_mem() {
    debug("Testing memory allocation...");
    void* p1 = alloc_page();
    void* h1 = kmalloc(128);
    debug("Page allocated at 0x%p", itoa((uint32_t)p1));
    debug("Heap allocated at 0x%p", itoa((uint32_t)h1));
    free_page(p1);
    kfree(h1);
    debug("Memory test complete. De-allocating memory from test.\n");
}

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
    char buf[256];
    (void)buf;
    //init registers as empty
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init();
    info("Initializing Memory Module");
    mem_init((atag_t*)atags);
    test_mem();
    
    // Mini Shell
    shell_run();
}