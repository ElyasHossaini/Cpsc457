# Validation

Validated on September 25, 2026 (America/Edmonton).

## Course Minix image

- Source appliance: `CPSC 457 - Minix Boilerplate.ova`, already present in Downloads.
- Extracted inner disk: `minix3.1.0-book-version-harddisk.img` from the appliance's
  `/home/cpsc457/cpsc457-minix-boilerplate-0.2.3/build` directory.
- OS: Minix 3.1.0, boot image revision 2, 32-bit i686.
- Emulator: QEMU 8.2.2 in the existing Ubuntu WSL distribution.
- Build: `cc cpsc457-a1.c -o ./cpsc457-a1` using the compiler inside Minix.
- Result: compilation succeeded; the assignment sample produced the expected
  values; **18 portable regression checks passed, zero failed**.
- Evidence: `validation/minix-test.txt`, exported from the running Minix filesystem.

The original running VirtualBox VM rejected the help guide's guest-control login.
Testing therefore used a separate copy of the exact Minix disk from the course
appliance. No claims are made about the original VM's current shared-folder,
login, or compiler configuration. Its files and settings were not changed.
The isolated Minix instance was shut down cleanly after testing.

## Supplemental Linux checks

- Environment: existing Ubuntu WSL; GCC 13.3.0.
- Strict compilation: `-std=c89 -Wall -Wextra -Werror -pedantic`.
- Portable tests: **18 passed, zero failed**.
- Exhaustive arithmetic: all indices 0 through 47 match an independent Python
  Fibonacci reference.
- Process trace: eight inputs create exactly eight children; every child writes
  to a pipe; only the parent writes stdout; all eight children are reaped.
- Invalid-input trace: no child processes are created.
- Fault injection: forcing the third `clone` or `pipe2` call to fail produces a
  diagnostic and nonzero exit, with both previously created children reaped.
- Evidence: `validation/linux-regression.txt` and `validation/linux-process-checks.txt`.

`clone`, `pipe2`, and `wait4` are the Linux syscalls used underneath this program's
portable `fork`, `pipe`, and `wait` library calls. Fault injection was performed
only in isolated test processes; system resource limits were not changed.

The Minix test shell does not support `set -u`; the portable test script avoids
that option. Python and strace tests are supplemental and run only on Linux.

## Limits

- Accepted Fibonacci indices: 0 through 47, to avoid 32-bit unsigned-long overflow.
- No testing was performed on the university's Linux servers.
- No assignment submission was made to D2L.

The setup installed QEMU, guestfs tools, a guestfs-compatible kernel, and strace
in Ubuntu WSL to perform these checks. An extracted appliance copy was used for
read-only access; no VM passwords or host security settings were changed.
