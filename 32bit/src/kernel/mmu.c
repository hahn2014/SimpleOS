/************************************************************
 *                                                          *
 *                ~ SimpleOS - mmu.c ~                      *
 *                                                          *
 *  Sets up a flat 1 GiB identity mapping using 1 MiB       *
 *  sections, then enables MMU, data/instruction caches and *
 *  branch predictor. L1 table fixed at 0x00100000.         *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Fine-grained (4 KiB) pages and device mappings    *
 ************************************************************/

#include <kernel/mmu.h>
#include <common/stdio.h>
#include <common/stdlib.h>

/* L1 translation table - must be 16 KiB aligned */
#define L1_TABLE_BASE   0x00100000UL
#define L1_SIZE         0x4000

/* 1 MiB section descriptor bits */
#define SECTION_AP_RW   (0b11 << 10)    /* Full read/write access */
#define SECTION_CB_11   (0b11 << 2)     /* Write-back, write-allocate */
#define SECTION_DOMAIN0 (0 << 5)
#define SECTION_XN      (0 << 4)        /* Execute never disabled */
#define SECTION_TYPE    (2 << 0)

#define DOMAIN_CLIENT   0b01           /* Domain 0 checked by page tables */

static uint32_t *l1_table = (uint32_t *)L1_TABLE_BASE;

/** Initialises the MMU with a 1 GiB identity mapping and enables caches */
void mmu_init(void) {
    uint32_t i;

    info("MMU init (L1 table at 0x%08X)", L1_TABLE_BASE);

    /* Clear entire L1 table */
    bzero(l1_table, L1_SIZE);

    /* Identity map first 1 GiB (1024 × 1 MiB sections) */
    for (i = 0; i < 1024; i++) {
        uint32_t phys = i << 20;
        l1_table[i] = phys | SECTION_AP_RW | SECTION_CB_11 |
                      SECTION_DOMAIN0 | SECTION_XN | SECTION_TYPE;
    }

    /* Full invalidate of TLB and instruction cache */
    asm volatile (
        "mcr p15, 0, %0, c8, c7, 0\n"   /* TLBIALL */
        "mcr p15, 0, %0, c7, c5, 0\n"   /* ICIALLU */
        "dsb\n"
        "isb\n"
        :: "r"(0)
    );

    /* Set translation table base and domain access */
    asm volatile("mcr p15, 0, %0, c2, c0, 0" : : "r"(L1_TABLE_BASE)); /* TTBR0 */
    asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r"(DOMAIN_CLIENT)); /* DACR */

    asm volatile("dsb");

    /* Enable MMU with caches off first (safer) */
    uint32_t sctlr;
    asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r"(sctlr));
    sctlr |= (1 << 0);                  /* MMU enable */
    sctlr &= ~((1 << 2) | (1 << 12));    /* Ensure caches off */
    asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r"(sctlr));
    asm volatile("isb");

    /* Now enable caches and branch predictor */
    asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r"(sctlr));
    sctlr |= (1 << 2)  /* D-cache */
           | (1 << 12) /* I-cache */
           | (1 << 11);/* Branch prediction */
    asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r"(sctlr));
    asm volatile("isb");

    info("MMU, caches and branch predictor enabled");
}