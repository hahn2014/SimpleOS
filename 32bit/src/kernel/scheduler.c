/************************************************************
 *                                                          *
 *             ~ SimpleOS - scheduler.c ~                   *
 *                                                          *
 *  Per-task register context only (shared kernel stack     *
 *      safe at this stage since no user mode/syscalls yet) *
 *      Low overhead, minimal copies only on switch.        *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 21 2026                          *
 *  ToDo:                                                   *
 ************************************************************/

#include <kernel/scheduler.h>
#include <common/stdio.h>
#include <kernel/mem.h>

extern uint32_t interrupt_context[16];

static task_t main_task;
task_t *current_task = NULL;

void scheduler_init(void) {
    current_task = &main_task;
    main_task.next = &main_task;
    main_task.name = "main";
    main_task.stack_base = NULL;  /* Uses boot IRQ stack */
    info("Scheduler initialized - preemptive round-robin with private IRQ stacks ready");
}

void task_create(void (*func)(void), const char *name) {
    task_t *t = (task_t *)kmalloc(sizeof(task_t));
    if (!t) panic("Out of heap for task struct");

    void *stack_page = alloc_page();
    if (!stack_page) panic("Out of memory for task stack");

    t->stack_base = stack_page;
    t->name = name;

    /* Zero all registers */
    for (int i = 0; i < 13; i++) t->context[i] = 0;

    /* Private IRQ stack top: 128 bytes headroom for top protection + 16-byte alignment (ABI safe) */
    uint32_t stack_top = (uint32_t)stack_page + PAGE_SIZE - 2048;
    stack_top &= ~0xF;  /* Force 16-byte alignment */

    t->context[13] = stack_top;

    /* Exception return address */
    t->context[14] = (uint32_t)func + 4;

    /* SPSR: IRQ mode, ARM, interrupts enabled on return */
    t->context[15] = 0x00000012;

    /* Insert into ready queue */
    t->next = current_task->next;
    current_task->next = t;

    info("Created task \"%s\" at %p (private IRQ stack at %p)", name, func, stack_page);
}

void schedule(void) {
    for (int i = 0; i < 16; i++) {
        current_task->context[i] = interrupt_context[i];
    }

    current_task = current_task->next;

    for (int i = 0; i < 16; i++) {
        interrupt_context[i] = current_task->context[i];
    }

    //debug("Context switch to %s", current_task->name);
}

size_t get_schedule(const char **out, size_t max) {
    if (!current_task || !out || max == 0)
        return 0;

    size_t count = 0;
    task_t *t = current_task;

    do {
        if (count >= max)
            break;

        out[count++] = t->name;
        t = t->next;
    } while (t != current_task);

    return count;
}