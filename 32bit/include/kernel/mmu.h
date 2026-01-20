/************************************************************
 *                                                          *
 *                 ~ SimpleOS - mmu.h ~                     *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Declaration of the Memory Management Unit initialiser.  *
 *  Used to set up translation tables and enable the MMU.   *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add fine-grained page mappings and device regions *
 ************************************************************/

#ifndef MMU_H
#define MMU_H

/** 
 * Initialises the MMU with a 1 GiB identity mapping using
 * 1 MiB sections, then enables the MMU, data/instruction
 * caches and branch predictor.
 */
void mmu_init(void);

#endif /* MMU_H */