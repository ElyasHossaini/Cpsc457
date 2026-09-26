/* CPSC 457 A1: one child and one result pipe per Fibonacci index.
 * Written in C89-compatible C for the course's Minix 3.1.0 compiler.
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ARGUMENTS 8
/* F(47) fits in a 32-bit unsigned long; F(48) does not. */
#define MAX_INDEX 47

