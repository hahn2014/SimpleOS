#ifndef STDIO_H
#define STDIO_H

#include <kernel/uart.h>
#include <common/stdlib.h>

char getc();
void putc(char c);
void puts(const char* str);
void gets(char* buf, int buflen);

#endif
