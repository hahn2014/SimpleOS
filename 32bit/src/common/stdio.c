/************************************************************
 *                                                          *
 *               ~ SimpleOS - stdio.c ~                     *
 *                                                          *
 *  Implementation of formatted I/O and helpers.            *
 *  All output goes through the early UART driver.          *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add buffer for interrupt-driven UART later        *
 ************************************************************/

#include <stdarg.h>
#include <common/stdio.h>

/** Reads a single character from UART (blocking) */
char getc() {
    return uart_getc();
}

/** Writes a single character to UART */
void putc(char c) {
    uart_putc(c);
}

/** Writes a null-terminated string */
void puts(const char* str) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        putc(str[i]);
    }
}

/** Reads a line with basic editing (backspace/delete) */
void gets(char* buf, int buflen) {
    int i = 0;
    char c;

    while ((c = getc()) != '\r' && buflen > 1) {
        if (c == '\b' || c == 0x7F) {            // Backspace or Delete
            if (i > 0) {
                i--;
                putc('\b');
                putc(' ');
                putc('\b');
            }
        } else if (c >= 32 && c <= 126) {        // Printable ASCII
            if (i < buflen - 1) {
                putc(c);
                buf[i++] = c;
            }
        }
    }

    putc('\r');
    putc('\n');
    buf[i] = '\0';
}

/** Prints a 32-bit value in hex with 0x prefix */
void puthex(uint32_t val) {
    const char *hexdigits = "0123456789ABCDEF";
    puts("0x");
    for (int shift = 28; shift >= 0; shift -= 4) {
        uint8_t digit = (val >> shift) & 0xF;
        putc(hexdigits[digit]);
    }
}

/* Coloured logging helpers */
void debug(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf(ANSI_FG_MAGENTA "[DEBUG] " ANSI_RESET COLOR_DEFAULT);
    vprintf(fmt, ap);
    printf(ANSI_RESET "\n");
    va_end(ap);
}

void info(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf(ANSI_FG_CYAN "[INFO] " ANSI_RESET COLOR_DEFAULT);
    vprintf(fmt, ap);
    printf(ANSI_RESET "\n");
    va_end(ap);
}

void warning(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf(ANSI_FG_YELLOW "[WARNING] " ANSI_RESET COLOR_DEFAULT);
    vprintf(fmt, ap);
    printf(ANSI_RESET "\n");
    va_end(ap);
}

void error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf(ANSI_FG_RED "[ERROR] " ANSI_RESET COLOR_DEFAULT);
    vprintf(fmt, ap);
    printf(ANSI_RESET "\n");
    va_end(ap);
}

void panic(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf(ANSI_BOLD ANSI_BG_RED "\n=== KERNEL PANIC ===\n" ANSI_RESET ANSI_FG_RED);
    vprintf(fmt, ap);
    printf(ANSI_RESET "\nSystem halted.\n");
    while (1) asm volatile("wfi");
    va_end(ap);
}

/** Internal helper to print a number in any base with padding */
static void print_number(unsigned long num, int base, int width, char pad_char, int upper) {
    char buf[32];
    const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    int i = 0;

    if (num == 0) {
        if (width > 1) {
            width--;
            while (width-- > 0) putc(pad_char);
        }
        putc('0');
        return;
    }

    while (num) {
        buf[i++] = digits[num % base];
        num /= base;
    }

    if (width > i) {
        width -= i;
        while (width-- > 0) putc(pad_char);
    }

    while (i--) putc(buf[i]);
}

/** Formatted output - supports %s %c %d %u %x %X %p %% */
void printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    for (;;) {
        char c = *fmt++;
        if (!c) break;

        if (c != '%') {
            putc(c);
            continue;
        }

        char pad_char = ' ';
        int width = 0;
        if (*fmt == '0') {
            pad_char = '0';
            fmt++;
        }
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt++ - '0');
        }

        c = *fmt++;
        if (c == 's') {
            const char *s = va_arg(ap, const char *);
            if (!s) s = "(null)";
            while (*s) putc(*s++);
        } else if (c == 'c') {
            putc(va_arg(ap, int));
        } else if (c == 'd') {
            int num = va_arg(ap, int);
            if (num < 0) {
                putc('-');
                num = -num;
            }
            print_number((unsigned)num, 10, width, pad_char, 0);
        } else if (c == 'u') {
            print_number(va_arg(ap, unsigned), 10, width, pad_char, 0);
        } else if (c == 'x') {
            print_number(va_arg(ap, unsigned), 16, width, pad_char, 0);
        } else if (c == 'X') {
            print_number(va_arg(ap, unsigned), 16, width, pad_char, 1);
        } else if (c == 'p') {
            puts("0x");
            print_number(va_arg(ap, unsigned long), 16, 8, '0', 0);
        } else if (c == '%') {
            putc('%');
        } else {
            putc('%');
            if (c) putc(c);
        }
    }

    va_end(ap);
}

/** Version of printf that takes a va_list - used by coloured helpers */
void vprintf(const char *fmt, va_list ap) {
    va_list ap_copy;
    va_copy(ap_copy, ap);

    /* Identical parsing logic to printf - reuse the same code path */
    for (;;) {
        char c = *fmt++;
        if (!c) break;

        if (c != '%') {
            putc(c);
            continue;
        }

        char pad_char = ' ';
        int width = 0;
        if (*fmt == '0') {
            pad_char = '0';
            fmt++;
        }
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt++ - '0');
        }

        c = *fmt++;
        if (c == 's') {
            const char *s = va_arg(ap_copy, const char *);
            if (!s) s = "(null)";
            while (*s) putc(*s++);
        } else if (c == 'c') {
            putc(va_arg(ap_copy, int));
        } else if (c == 'd') {
            int num = va_arg(ap_copy, int);
            if (num < 0) {
                putc('-');
                num = -num;
            }
            print_number((unsigned)num, 10, width, pad_char, 0);
        } else if (c == 'u') {
            print_number(va_arg(ap_copy, unsigned), 10, width, pad_char, 0);
        } else if (c == 'x') {
            print_number(va_arg(ap_copy, unsigned), 16, width, pad_char, 0);
        } else if (c == 'X') {
            print_number(va_arg(ap_copy, unsigned), 16, width, pad_char, 1);
        } else if (c == 'p') {
            puts("0x");
            print_number(va_arg(ap_copy, unsigned long), 16, 8, '0', 0);
        } else if (c == '%') {
            putc('%');
        } else {
            putc('%');
            if (c) putc(c);
        }
    }

    va_end(ap_copy);
}