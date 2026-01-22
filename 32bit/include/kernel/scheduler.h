/************************************************************
 *                                                          *
 *               ~ SimpleOS - scheduler.h ~                 *
 *                                                          *
 *  Declarations for the task structure and scheduler       *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 21 2026                          *
 *  ToDo: Add interrupt-driven buffering                    *
 ************************************************************/

#ifndef _KERNEL_SCHEDULER_H
#define _KERNEL_SCHEDULER_H

#include <stdint.h>
#include <stddef.h>

typedef struct task {
    struct task *next;
    uint32_t context[16];      /* 0-11: r0-r11
                                  12: r12
                                  13: r13_irq (private IRQ stack pointer)
                                  14: LR_irq (exception return)
                                  15: SPSR */
    void *stack_base;          /* Page for free_page on destroy (future) */
    const char *name;
} task_t;

void scheduler_init(void);
void task_create(void (*func)(void), const char *name);
void schedule(void);
size_t get_schedule(const char **out, size_t max);

#endif