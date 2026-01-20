/************************************************************
 *                                                          *
 *                ~ SimpleOS - stdio.h ~                    *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Formatted I/O and debug printing built on the UART.     *
 *  Provides printf-family functions suitable for kernel.   *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add floating-point support and sprintf            *
 ************************************************************/

#ifndef STDIO_H
#define STDIO_H

#include <kernel/uart.h>
#include <common/stdlib.h>
#include <stdarg.h>

/* ANSI colour escape sequences */
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

#define COLOR_DEFAULT   ANSI_FG_WHITE

/** Reads one character from UART (blocking) */
char getc();

/** Writes one character to UART */
void putc(char c);

/** Writes a null-terminated string to UART */
void puts(const char* str);

/** Reads a line into buf (with simple backspace handling) */
void gets(char* buf, int buflen);

/** Prints a 32-bit value in hexadecimal with 0x prefix */
void puthex(uint32_t val);

/** Coloured debug output */
void debug(const char *fmt, ...);

/** Coloured info output */
void info(const char *fmt, ...);

/** Coloured warning output */
void warning(const char *fmt, ...);

/** Coloured error output */
void error(const char *fmt, ...);

/** Kernel panic - prints message and halts */
void panic(const char *fmt, ...);

/** Standard printf */
void printf(const char *fmt, ...);

/** Internal vprintf used by the coloured helpers */
void vprintf(const char *fmt, va_list ap);

#endif /* STDIO_H */