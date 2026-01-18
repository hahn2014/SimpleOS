#ifndef STDIO_H
#define STDIO_H

#include <kernel/uart.h>
#include <common/stdlib.h>

char getc();
void putc(char c);
void puts(const char* str);
void gets(char* buf, int buflen);
void puthex(uint32_t val);
void debug(const char *fmt, ...);
void warning(const char* msg);
void info(const char* msg);
void error(const char* msg);
void panic(const char* msg);
void printf(const char *fmt, ...);

#endif
