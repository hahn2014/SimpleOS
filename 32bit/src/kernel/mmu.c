#include <kernel/mmu.h>
#include <common/stdio.h>
#include <common/stdlib.h>

/* L1 table must be 16KB aligned for Cortex-A7 TTBR0 */
#define L1_TABLE_BASE   0x00100000UL    /* 1MB - safe, fixed, after typical kernel */
#define L1_SIZE         0x4000          /* 16KB for full 4GB coverage */

/* Section (1MB) flags */
#define SECTION_AP_RW   (0b11 << 10)    /* Priv RW, User RW */
#define SECTION_CB_11   (0b11 << 2)     /* Write-back, write-allocate */
#define SECTION_DOMAIN0 (0 << 5)
#define SECTION_XN      (0 << 4)        /* Execute allowed */
#define SECTION_TYPE    (2 << 0)

/* Domain 0 = Client (checked by table) */
#define DOMAIN_CLIENT   0b01

static uint32_t *l1_table = (uint32_t *)L1_TABLE_BASE;

void mmu_init(void) {
    uint32_t i;

    info("MMU init (L1 table at 0x%08X)", L1_TABLE_BASE);

    bzero(l1_table, L1_SIZE);

    /* Identity map 0-1GB (1MB sections) */
    for (i = 0; i < 1024; i++) {
        uint32_t phys = i << 20;
        l1_table[i] = phys | SECTION_AP_RW | SECTION_CB_11 | SECTION_DOMAIN0 | SECTION_XN | SECTION_TYPE;
    }

    /* Full invalidate */
    asm volatile(
        "mcr p15, 0, %0, c8, c7, 0\n"   /* TLBIALL */
        "mcr p15, 0, %0, c7, c5, 0\n"   /* ICIALLU */
        "dsb\n"
        "isb\n"
        :: "r"(0)
    );
    debug("[mmu.c] - MMU has been safely reset");

    /* Set TTBR0 and domain */
    asm volatile("mcr p15, 0, %0, c2, c0, 0" : : "r"(L1_TABLE_BASE));
    asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r"(DOMAIN_CLIENT));
    asm volatile("dsb");
    debug("[mmu.c] - TTBR0 and Domain set sucessfully");

    /* Enable MMU only (caches off first for safety) */
    uint32_t sctlr;
    asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r"(sctlr));
    sctlr |= (1 << 0);   /* MMU */
    sctlr &= ~( (1 << 2) | (1 << 12) );  /* Ensure caches off */
    asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r"(sctlr));
    asm volatile("isb");

    debug("[mmu.c] - MMU enabled (caches off)");

    /* Now enable caches + branch predictor */
    asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r"(sctlr));
    sctlr |= (1 << 2)   /* D-cache */
           | (1 << 12)  /* I-cache */
           | (1 << 11); /* Branch prediction */
    asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r"(sctlr));
    asm volatile("isb");

    info("MMU, Caches and branch predictor enabled");
}