/************************************************************
 *                                                          *
 *               ~ SimpleOS - stdlib.h ~                    *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Minimal standard library replacements for bare-metal.   *
 *  No dependencies on libc.                                *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Optimise memcpy/memset with word stores           *
 ************************************************************/

#ifndef STDLIB_H
#define STDLIB_H

/**
 * Copies 'bytes' bytes from src to dest.
 * No overlap checking - behaviour undefined if regions overlap.
 */
void memcpy(void* dest, void* src, int bytes);

/**
 * Zeroes 'bytes' bytes starting at dest.
 */
void bzero(void* dest, int bytes);

/**
 * Converts integer i to decimal string.
 * Returns pointer to static internal buffer (not re-entrant).
 */
char* itoa(int i);

/**
 * Standard string compare.
 * Returns <0, 0 or >0 according to lexicographical order.
 */
int strcmp(const char *s1, const char *s2);

#endif /* STDLIB_H */