/************************************************************
 *                                                          *
 *                 ~ SimpleOS - mem.h ~                     *
 *                                                          *
 *  Physical page allocator and simple kernel heap.         *
 *  Pages are tracked with a bitmap-like structure; the     *
 *  heap uses a best-fit linked-list allocator.             *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Replace with buddy allocator and slab caches     *
 ************************************************************/

#ifndef MEM_H
#define MEM_H

#include <kernel/atag.h>
#include <kernel/list.h>
#include <common/stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define KERNEL_HEAP_SIZE (1024 * 1024)

/** Flags for each physical page */
typedef struct {
    uint8_t allocated: 1;         // Page is in use
    uint8_t kernel_page: 1;       // Page belongs to kernel image / structures
    uint8_t kernel_heap_page: 1;  // Page is part of the kernel heap region
    uint32_t reserved: 29;
} page_flags_t;

/** Physical page descriptor */
typedef struct page {
    uint32_t vaddr_mapped;        // Virtual address this page is mapped to (if any)
    page_flags_t flags;
    DEFINE_LINK(page);            // Doubly-linked list node pointers
} page_t;

/** Kernel heap segment header (16-byte aligned) */
typedef struct heap_segment {
    struct heap_segment* next;
    struct heap_segment* prev;
    uint32_t is_allocated;        // 1 = allocated, 0 = free
    uint32_t segment_size;        // Size including this header
} heap_segment_t;

/* Public API */

/**
 * Initialises the page allocator and kernel heap using memory
 * information from the ATAGs.
 */
void mem_init(atag_t* atags);

/**
 * Allocates and zeroes one physical page.
 * Returns virtual address (identity mapped) or NULL if OOM.
 */
void* alloc_page(void);

/**
 * Frees a previously allocated page.
 */
void free_page(void* ptr);

/**
 * Allocates a block from the kernel heap (best-fit).
 * Size is rounded up to 16-byte alignment.
 */
void* kmalloc(uint32_t bytes);

/**
 * Frees a block allocated with kmalloc.
 */
void kfree(void* ptr);

/**
 * Returns total number of physical pages in the system.
 */
uint32_t get_total_pages(void);

/**
 * Returns number of currently free pages.
 */
uint32_t get_free_pages(void);

/**
 * Simple sanity test of page and heap allocators.
 */
void test_mem(void);

#endif /* MEM_H */