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

/* Accept only nonempty decimal strings, checking the bound before overflow. */
static int parse_index(const char *text, unsigned int *index)
{
    unsigned int value;
    unsigned int digit;

    value = 0;
    if (*text == '\0')
        return -1;
    while (*text != '\0') {
        if (*text < '0' || *text > '9')
            return -1;
        digit = (unsigned int)(*text - '0');
        if (value > (MAX_INDEX - digit) / 10)
            return -1;
        value = value * 10 + digit;
        ++text;
    }
    *index = value;
    return 0;
}

