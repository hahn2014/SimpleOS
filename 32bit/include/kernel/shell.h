/************************************************************
 *                                                          *
 *                ~ SimpleOS - shell.h ~                    *
 *                     version 0.04-alpha                   *
 *                                                          *
 *  Declaration of the interactive command shell.           *
 *  The shell is the main user interface after boot.        *
 *                                                          *
 *  License: MIT                                            *
 *  Last Modified: January 19 2026                          *
 *  ToDo: Add tab completion and command history            *
 ************************************************************/

#ifndef SHELL_H
#define SHELL_H

/**
 * Starts the interactive REPL shell.
 * Prints a banner, displays help and enters an infinite
 * command loop. Never returns.
 */
void shell_run(void);

#endif /* SHELL_H */