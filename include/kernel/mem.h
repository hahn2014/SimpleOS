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
	uint8_t kernel_heap_page: 1;	// This page is a part of the kernel
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

static void heap_init(uint32_t heap_start);
/**
 * impliment kmalloc as a linked list of allocated segments.
 * Segments should be 4 byte aligned.
 * Use best fit algorithm to find an allocation
 */
typedef struct heap_segment {
    struct heap_segment* next;
    struct heap_segment* prev;
    uint32_t is_allocated;
    uint32_t segment_size;  // Includes this header
} heap_segment_t;

static heap_segment_t* heap_segment_list_head;

/**
 * End Heap Stuff
 */


void mem_init(atag_t* atags);
void* alloc_page(void);
void free_page(void* ptr);
void heap_init(uint32_t heap_start);
void* kmalloc(uint32_t bytes);
void kfree(void* ptr);

#endif
