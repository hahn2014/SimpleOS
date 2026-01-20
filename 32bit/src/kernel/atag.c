/************************************************************
 *                                                          *
 *               ~ SimpleOS - atag.c ~                      *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Tiny ATAG parser - extracts memory size from firmware   *
 *  provided ATAG list.                                     *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Parse additional useful tags (cmdline, initrd)    *
 ************************************************************/

#include <kernel/atag.h>

/** Scans ATAG list and returns RAM size in bytes (0 if no MEM tag found) */
uint32_t get_mem_size(atag_t *tag) {
    while (tag->tag != NONE) {
        if (tag->tag == MEM) {
            return tag->mem.size;
        }
        tag = (atag_t *)((uint32_t *)tag + tag->tag_size);
    }
    return 0;
}