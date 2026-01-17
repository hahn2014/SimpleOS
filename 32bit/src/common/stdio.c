#include <common/stdio.h>

char getc() {
    return uart_getc();
}

void putc(char c) {
    uart_putc(c);
}

void puts(const char* str) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        putc(str[i]);
    }
}

void gets(char* buf, int buflen) {
    int i;
    char c;

    for (i = 0; (c = getc()) != '\r' && buflen > 1; i++, buflen--) {
        putc(c);
        buf[i] = c;
    }

    putc('\n');
    if (c == '\n') {
        buf[i] = '\0';
    } else {
        buf[buflen - 1] = '\0';
    }
}

void puthex(uint32_t val) {
    const char *hexdigits = "0123456789ABCDEF";
    puts("0x");
    for (int shift = 28; shift >= 0; shift -= 4) {
        uint8_t digit = (val >> shift) & 0xF;
        putc(hexdigits[digit]);
    }
}

void debug(const char* msg) {
    puts("[DEBUG] ");
    puts(msg);
    puts("\n");
}

void info(const char* msg) {
    puts("[INFO] ");
    puts(msg);
    puts("\n");
}

void warning(const char* msg) {
    puts("[WARNING] ");
    puts(msg);
    puts("\n");
}

void error(const char* msg) {
    puts("[ERROR] ");
    puts(msg);
    puts("\n");
}

void panic(const char* msg) {
    puts("\n=== KERNEL PANIC ===\n");
    puts(msg);
    puts("\nSystem halted.\n");
    while (1) {
        asm volatile("wfi");
    }
}
