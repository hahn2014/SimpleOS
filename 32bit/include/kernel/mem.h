#ifndef MEM_H
#define MEM_H

#include <kernel/atag.h>
#include <kernel/list.h>
#include <common/stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define KERNEL_HEAP_SIZE (1024 * 1024)

typedef struct {
	uint8_t allocated: 1;			// This page is allocated to something
	uint8_t kernel_page: 1;			// This page is a part of the kernel
	uint8_t kernel_heap_page: 1;	// This page is a part of the kernel heap
	uint32_t reserved: 29;
} page_flags_t;

typedef struct page {
	uint32_t vaddr_mapped;	// The virtual address that maps to this page
	page_flags_t flags;
	DEFINE_LINK(page);
} page_t;

/**
 * Heap Stuff
 */

/**
 * kmalloc is implemented as a linked list of allocated segments.
 * Segments are 16-byte aligned (for potential future use).
 * Best-fit algorithm is used.
 */
typedef struct heap_segment {
    struct heap_segment* next;
    struct heap_segment* prev;
    uint32_t is_allocated;
    uint32_t segment_size;  // Includes this header
} heap_segment_t;

/**
 * End Heap Stuff
 */

void mem_init(atag_t* atags);
void* alloc_page(void);
void free_page(void* ptr);
void* kmalloc(uint32_t bytes);
void kfree(void* ptr);
uint32_t get_total_pages(void);
uint32_t get_free_pages(void);
void test_mem(void);

#endif