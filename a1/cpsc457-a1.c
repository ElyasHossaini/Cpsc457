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

static unsigned long fibonacci(unsigned int index)
{
    unsigned long previous;
    unsigned long current;
    unsigned long next;
    unsigned int i;

    if (index == 0)
        return 0;
    previous = 0;
    current = 1;
    for (i = 2; i <= index; ++i) {
        next = previous + current;
        previous = current;
        current = next;
    }
    return current;
}

/* A pipe is a byte stream: retry interruptions and finish partial transfers. */
static int write_result(int fd, unsigned long result)
{
    const char *bytes;
    size_t remaining;
    ssize_t amount;

    bytes = (const char *)&result;
    remaining = sizeof(result);
    while (remaining != 0) {
        amount = write(fd, bytes, remaining);
        if (amount < 0 && errno == EINTR)
            continue;
        if (amount <= 0)
            return -1;
        bytes += amount;
        remaining -= (size_t)amount;
    }
    return 0;
}

static int read_result(int fd, unsigned long *result)
{
    char *bytes;
    size_t remaining;
    ssize_t amount;

    bytes = (char *)result;
    remaining = sizeof(*result);
    while (remaining != 0) {
        amount = read(fd, bytes, remaining);
        if (amount < 0 && errno == EINTR)
            continue;
        if (amount <= 0)
            return -1;
        bytes += amount;
        remaining -= (size_t)amount;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    unsigned int indices[MAX_ARGUMENTS];
    pid_t children[MAX_ARGUMENTS];
    int readers[MAX_ARGUMENTS];
    unsigned long results[MAX_ARGUMENTS];
    int received[MAX_ARGUMENTS];
    int pipefd[2];
    int requested;
    int started;
    int failed;
    int status;
    int i;
    int j;
    pid_t child;
    pid_t waited;

    requested = argc - 1;
    if (requested < 1 || requested > MAX_ARGUMENTS) {
        fprintf(stderr, "Usage: %s index [index ...] (1 to 8 indices, 0 to 47)\n",
                argv[0]);
        return EXIT_FAILURE;
    }
    /* Validate every argument before spawning any processes. */
    for (i = 0; i < requested; ++i) {
        if (parse_index(argv[i + 1], &indices[i]) != 0) {
            fprintf(stderr, "Invalid index '%s': expected an integer from 0 to 47.\n",
                    argv[i + 1]);
            return EXIT_FAILURE;
        }
    }

