#include <kernel/mem.h>
#include <common/stdio.h>

extern uint8_t __end;
static uint32_t num_pages;

DEFINE_LIST(page);
IMPLEMENT_LIST(page);

static page_t* all_pages_array;
page_list_t free_pages;

static heap_segment_t *heap_segment_list_head;
static void heap_init(uint32_t heap_start);

void mem_init(atag_t* atags) {
    uint32_t mem_size = 1UL << 30;  // Fixed 1 GiB - exact match for Raspberry Pi 2B and qemu -m 1024
    uint32_t page_array_len, kernel_pages, page_array_end, i;

    /* Silence unused parameter warning and print the (likely garbage) ATAG pointer for debug */
    (void)atags;

    puts("[DEBUG] mem_init: raw ATAGs pointer from r2 = ");
    puthex((uint32_t)atags);
    puts("\n");

    puts("[DEBUG] Using fixed 1 GiB physical memory size (safe for QEMU and real Pi 2B)\n");

    puts("[DEBUG] Total memory = ");
    puthex(mem_size);
    puts(" bytes\n");

    num_pages = mem_size / PAGE_SIZE;

    puts("[DEBUG] Total pages = ");
    puthex(num_pages);
    puts("\n");

    page_array_len = sizeof(page_t) * num_pages;
    all_pages_array = (page_t*)&__end;

    /* Zero the entire page metadata array */
    bzero(all_pages_array, page_array_len);

    INITIALIZE_LIST(free_pages);

    /* Mark kernel image pages */
    kernel_pages = ((uint32_t)&__end) / PAGE_SIZE;
    for (i = 0; i < kernel_pages; i++) {
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;
        all_pages_array[i].flags.allocated = 1;
        all_pages_array[i].flags.kernel_page = 1;
    }

    /* Reserve kernel heap pages */
    for (; i < kernel_pages + (KERNEL_HEAP_SIZE / PAGE_SIZE); i++) {
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;
        all_pages_array[i].flags.allocated = 1;
        all_pages_array[i].flags.kernel_heap_page = 1;
    }

    /* Add remaining pages to free list */
    for (; i < num_pages; i++) {
        all_pages_array[i].flags.allocated = 0;
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;  // Optional but consistent
        append_page_list(&free_pages, &all_pages_array[i]);
    }

    page_array_end = (uint32_t)&__end + page_array_len;
    heap_init(page_array_end);

    puts("[DEBUG] Memory initialization complete. Free pages = ");
    puthex(size_page_list(&free_pages));
    puts("\n");
}

void* alloc_page(void) {
    page_t* page;
    void* page_mem;

    if (size_page_list(&free_pages) == 0) {
        return 0; //if there is no more allocatable space remaining
    }

    // Get a free page
    page = pop_page_list(&free_pages);
    page->flags.kernel_page = 1;
    page->flags.allocated = 1;

    // Get the address the physical page metadata refers to
    page_mem = (void*)((page - all_pages_array) * PAGE_SIZE);

    // Zero out the page, big security flaw to not do this :)
    bzero(page_mem, PAGE_SIZE);

    return page_mem;
}

void free_page(void* ptr) {
    page_t* page;

    // Get page metadata from the physical address
    page = all_pages_array + ((uint32_t)ptr / PAGE_SIZE);

    // Mark the page as free
    page->flags.allocated = 0;
    append_page_list(&free_pages, page);
}


static void heap_init(uint32_t heap_start) {
    heap_segment_list_head = (heap_segment_t*)heap_start;
    bzero(heap_segment_list_head, sizeof(heap_segment_t));
    heap_segment_list_head->segment_size = KERNEL_HEAP_SIZE;
}

void* kmalloc(uint32_t bytes) {
    heap_segment_t *curr, *best = NULL;
    int diff, best_diff = 0x7fffffff; // Max signed int

    // Add the header to the number of bytes we need and make the size 4 byte aligned
    bytes += sizeof(heap_segment_t);
    bytes += bytes % 16 ? 16 - (bytes % 16) : 0;

    // Find the allocation that is closest in size to this request
    for (curr = heap_segment_list_head; curr != NULL; curr = curr->next) {
        diff = curr->segment_size - bytes;
        if (!curr->is_allocated && diff < best_diff && diff >= 0) {
            best = curr;
            best_diff = diff;
        }
    }

    // There must be no free memory right now :(
    if (best == NULL)
        return NULL;

    // If the best difference we could come up with was large, split up this segment into two.
    // Since our segment headers are rather large, the criterion for splitting the segment is that
    // when split, the segment not being requested should be twice a header size
    if (best_diff > (int)(2 * sizeof(heap_segment_t))) {
        bzero(((void*)(best)) + bytes, sizeof(heap_segment_t));
        curr = best->next;
        best->next = ((void*)(best)) + bytes;
        best->next->next = curr;
        best->next->prev = best;
        best->next->segment_size = best->segment_size - bytes;
        best->segment_size = bytes;
    }

    best->is_allocated = 1;

    return best + 1;
}

void kfree(void* ptr) {
    heap_segment_t* seg;

    if (!ptr)
        return;

    seg = ptr - sizeof(heap_segment_t);
    seg->is_allocated = 0;

    // try to coalesce segements to the left
    while(seg->prev != NULL && !seg->prev->is_allocated) {
        seg->prev->next = seg->next;
        seg->next->prev = seg->prev;
        seg->prev->segment_size += seg->segment_size;
        seg = seg->prev;
    }
    // try to coalesce segments to the right
    while(seg->next != NULL && !seg->next->is_allocated) {
        seg->next->next->prev = seg;
        seg->next = seg->next->next;
        seg->segment_size += seg->next->segment_size;
    }
}
