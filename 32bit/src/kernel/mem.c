/************************************************************
 *                                                          *
 *                ~ SimpleOS - mem.c ~                      *
 *                                                          *
 *  Physical page allocator + simple kernel heap (best-fit  *
 *  with splitting and coalescing). Memory size discovered  *
 *  via ATAGs.                                              *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Buddy allocator for pages, slab for small objects *
 ************************************************************/

#include <kernel/mem.h>
#include <kernel/atag.h>
#include <common/stdio.h>

extern uint8_t __end;

static uint32_t num_pages;
DEFINE_LIST(page);
IMPLEMENT_LIST(page);

static page_t *all_pages_array;
page_list_t free_pages;

static heap_segment_t *heap_segment_list_head;

static void heap_init(uint32_t heap_start);

/** Initialises page allocator and kernel heap using ATAG-provided memory size */
void mem_init(atag_t *atags) {
    info("Initializing Memory Module");

    uint32_t mem_size = get_mem_size(atags);
    if (mem_size == 0) {
        debug("[mem.c] No MEM ATAG found - falling back to 1 GiB");
        mem_size = 1UL << 30;
    }

    debug("[mem.c] Detected memory size = 0x%08X (%u MiB)", mem_size, mem_size >> 20);

    num_pages = mem_size / PAGE_SIZE;

    uint32_t page_array_len = sizeof(page_t) * num_pages;
    all_pages_array = (page_t *)&__end;

    bzero(all_pages_array, page_array_len);
    INITIALIZE_LIST(free_pages);

    uint32_t kernel_pages = ((uint32_t)&__end + page_array_len + KERNEL_HEAP_SIZE - 1) / PAGE_SIZE;

    for (uint32_t i = 0; i < kernel_pages; i++) {
        all_pages_array[i].flags.allocated = 1;
        all_pages_array[i].flags.kernel_page = 1;
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;
    }

    for (uint32_t i = kernel_pages; i < num_pages; i++) {
        all_pages_array[i].flags.allocated = 0;
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;
        append_page_list(&free_pages, &all_pages_array[i]);
    }

    uint32_t heap_start = (uint32_t)&__end + page_array_len;
    heap_init(heap_start);

    info("Memory init complete - %u free pages", size_page_list(&free_pages));
}

/** Allocates and zeroes a single physical page (returns virtual address) */
void *alloc_page(void) {
    if (size_page_list(&free_pages) == 0) return NULL;

    page_t *page = pop_page_list(&free_pages);
    page->flags.allocated = 1;
    page->flags.kernel_page = 1;

    void *addr = (void *)((page - all_pages_array) * PAGE_SIZE);
    bzero(addr, PAGE_SIZE);
    return addr;
}

/** Frees a previously allocated page */
void free_page(void *ptr) {
    if (!ptr) return;
    page_t *page = all_pages_array + ((uint32_t)ptr / PAGE_SIZE);
    page->flags.allocated = 0;
    append_page_list(&free_pages, page);
}

/** Initialises the fixed-size kernel heap region with a single free segment */
static void heap_init(uint32_t heap_start) {
    heap_segment_list_head = (heap_segment_t *)heap_start;
    heap_segment_list_head->segment_size = KERNEL_HEAP_SIZE;
    heap_segment_list_head->is_allocated = 0;
    heap_segment_list_head->next = NULL;
    heap_segment_list_head->prev = NULL;
}

/** Allocates a 16-byte aligned block from the kernel heap (best-fit) */
void *kmalloc(uint32_t bytes) {
    bytes += sizeof(heap_segment_t);
    bytes = (bytes + 15) & ~15;

    heap_segment_t *best = NULL;
    int best_diff = 0x7FFFFFFF;

    for (heap_segment_t *cur = heap_segment_list_head; cur; cur = cur->next) {
        if (cur->is_allocated) continue;
        int diff = cur->segment_size - bytes;
        if (diff >= 0 && diff < best_diff) {
            best = cur;
            best_diff = diff;
        }
    }

    if (!best) return NULL;

    if (best_diff > (int)(2 * sizeof(heap_segment_t))) {
        heap_segment_t *new_seg = (heap_segment_t *)((uint8_t *)best + bytes);
        new_seg->segment_size = best->segment_size - bytes;
        new_seg->is_allocated = 0;
        new_seg->next = best->next;
        new_seg->prev = best;
        if (best->next) best->next->prev = new_seg;
        best->next = new_seg;
        best->segment_size = bytes;
    }

    best->is_allocated = 1;
    return best + 1;
}

/** Frees a heap block and coalesces adjacent free segments */
void kfree(void *ptr) {
    if (!ptr) return;

    heap_segment_t *seg = ((heap_segment_t *)ptr) - 1;
    seg->is_allocated = 0;

    if (seg->prev && !seg->prev->is_allocated) {
        seg->prev->segment_size += seg->segment_size;
        seg->prev->next = seg->next;
        if (seg->next) seg->next->prev = seg->prev;
        seg = seg->prev;
    }

    if (seg->next && !seg->next->is_allocated) {
        seg->segment_size += seg->next->segment_size;
        seg->next = seg->next->next;
        if (seg->next) seg->next->prev = seg;
    }
}

/** Returns total number of physical pages detected */
uint32_t get_total_pages(void) { return num_pages; }

/** Returns number of currently free pages */
uint32_t get_free_pages(void)  { return size_page_list(&free_pages); }

/** Simple sanity test of page and heap allocators */
void test_mem(void) {
    info("Testing memory allocation...");
    void *p = alloc_page();
    void *h = kmalloc(128);
    info("Page at %p, heap at %p", p, h);
    free_page(p);
    kfree(h);
    info("Memory test passed");
}