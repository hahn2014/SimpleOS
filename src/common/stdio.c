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
