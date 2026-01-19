#ifndef STDIO_H
#define STDIO_H

#include <kernel/uart.h>
#include <common/stdlib.h>
#include <stdarg.h>

#define ANSI_RESET      "\033[0m"
#define ANSI_BOLD       "\033[1m"
#define ANSI_FG_BLACK   "\033[30m"
#define ANSI_FG_RED     "\033[31m"
#define ANSI_FG_GREEN   "\033[32m"
#define ANSI_FG_YELLOW  "\033[33m"
#define ANSI_FG_BLUE    "\033[34m"
#define ANSI_FG_MAGENTA "\033[35m"
#define ANSI_FG_CYAN    "\033[36m"
#define ANSI_FG_WHITE   "\033[37m"
#define ANSI_BG_RED     "\033[41m"
//default plaintext color, to be customizable w/ user profiles
#define COLOR_DEFAULT   ANSI_FG_WHITE

char getc();
void putc(char c);
void puts(const char* str);
void gets(char* buf, int buflen);
void puthex(uint32_t val);
void debug(const char *fmt, ...);
void warning(const char *fmt, ...);
void info(const char *fmt, ...);
void error(const char *fmt, ...);
void panic(const char *fmt, ...);
void printf(const char *fmt, ...);
void vprintf(const char *fmt, va_list ap);

#endif
