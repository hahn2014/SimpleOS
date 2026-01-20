/************************************************************
 *                                                          *
 *                ~ SimpleOS - atag.h ~                     *
 *                                                          *
 *  ATAG parsing structures and helper. The firmware        *
 *  passes boot information via ATAGs on older Pi models.   *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add parsing for CMDLINE and INITRD tags           *
 ************************************************************/

#ifndef ATAG_H
#define ATAG_H

#include <stdint.h>

typedef enum {
    NONE    = 0x00000000,
    CORE    = 0x54410001,
    MEM     = 0x54410002,
    INITRD2 = 0x54420005,
    CMDLINE = 0x54410009,
} atag_tag_t;

/** Memory size/tag payload */
typedef struct {
    uint32_t size;   // In bytes
    uint32_t start;  // Physical start address
} mem_t;

/** Initrd2 payload */
typedef struct {
    uint32_t start;
    uint32_t size;
} initrd2_t;

/** Command line payload */
typedef struct {
    char line[1];    // Variable length
} cmdline_t;

/** Generic ATAG structure */
typedef struct atag {
    uint32_t tag_size;   // Size of tag in words (including this field)
    atag_tag_t tag;
    union {
        mem_t mem;
        initrd2_t initrd2;
        cmdline_t cmdline;
    };
} atag_t;

/**
 * Scans the ATAG list for a MEM tag and returns the detected
 * RAM size in bytes. Returns 0 if no MEM tag is found.
 */
uint32_t get_mem_size(atag_t* atags);

#endif /* ATAG_H */