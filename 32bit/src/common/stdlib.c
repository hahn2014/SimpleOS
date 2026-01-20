/************************************************************
 *                                                          *
 *              ~ SimpleOS - stdlib.c ~                     *
 *                                                          *
 *  Minimal implementations of common libc functions.       *
 *  Optimised for size rather than maximum speed.           *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add memset and word-aligned memcpy optimisation   *
 ************************************************************/

#include <common/stdlib.h>

/** Simple byte-by-byte memory copy */
void memcpy(void* dest, void* src, int bytes) {
    char *d = dest, *s = src;
    while (bytes--) {
        *d++ = *s++;
    }
}

/** Zero a block of memory */
void bzero(void* dest, int bytes) {
    char* d = dest;
    while (bytes--) {
        *d++ = 0;
    }
}

/** Integer to decimal string - uses static buffer (not re-entrant) */
char* itoa(int i) {
    static char intbuf[12];
    int j = 0, isneg = 0;

    if (i == 0) {
        intbuf[0] = '0';
        intbuf[1] = '\0';
        return intbuf;
    }

    if (i < 0) {
        isneg = 1;
        i = -i;
    }

    while (i != 0) {
       intbuf[j++] = '0' + (i % 10);
       i /= 10;
    }

    if (isneg)
        intbuf[j++] = '-';

    intbuf[j] = '\0';
    /* Reverse */
    j--;
    int start = 0;
    while (start < j) {
        char tmp = intbuf[start];
        intbuf[start] = intbuf[j];
        intbuf[j] = tmp;
        start++;
        j--;
    }

    return intbuf;
}

/** Standard lexicographical string compare */
int strcmp(const char *s1, const char *s2) {
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    while (*p1 && (*p1 == *p2)) {
        p1++;
        p2++;
    }

    return (*p1 > *p2) ? 1 : (*p1 < *p2) ? -1 : 0;
}