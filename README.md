# CPSC 457 - Assignment 1

A C program that computes Fibonacci numbers in child processes and returns the
results to the parent through pipes. Only the parent prints the final results.

## Compile and run in Minix

Place the source in `/usr/src/home/a1`, then run:

```sh
cd /usr/src/home/a1
cc cpsc457-a1.c -o ./cpsc457-a1
./cpsc457-a1 3 5 2 9 20
```

Example output (process IDs vary):

```text
Child Process (PID 100) F{3} = 2
Child Process (PID 101) F{5} = 5
Child Process (PID 102) F{2} = 1
Child Process (PID 103) F{9} = 34
Child Process (PID 104) F{20} = 6765
```

The program accepts **one to eight indices from 0 through 47 inclusive**.
Arguments may be unordered or repeated; leading zeroes are accepted. Negative
numbers, signs, fractions, whitespace, empty strings, nonnumeric input, too many
arguments, and values above 47 produce an error on standard error and a nonzero
exit status. All input is checked before creating any children.

The assignment does not specify a numeric range. This implementation explicitly
uses a 32-bit-safe `unsigned long` range: F(47) is 2971215073, while F(48) is
4807526976 and exceeds a 32-bit unsigned long. The same range is enforced on
64-bit Linux so behavior matches Minix. Results are exact within this range.

## Transfer from the graphical course VM

1. Put this directory's source and `tests` folder under
   `<Minix root>/src/home/a1` in Fedora. The help guide identifies the Minix root
   as `/home/cpsc457/cpsc457-minix-boilerplate-0.2.3`.
2. With the inner Minix instance shut down, enter `<Minix root>/build` and run
   `./copy_src_files.sh`. It copies the source tree into the Minix disk.
3. Start Minix using `./run_vm.sh` and log in as `root`, as described in the help
   guide. Compile using the commands above.
4. Repeat the copy step after source edits, while Minix is stopped. Keep source
   and shell-script line endings as LF when editing on Windows.

A1 is a user program: **no kernel rebuild is required**. To stop Minix cleanly,
the help guide specifies `shutdown`, followed by `off` at the boot monitor.

## Process and pipe design

- The parent creates a dedicated pipe before each `fork()`.
- Each child closes its read end and any earlier inherited read descriptors,
  computes exactly one Fibonacci number, writes it to its own pipe, and calls
  `_exit()`. Children never reach another iteration of the fork loop.
- The parent immediately closes each write end. It starts all requested children
  before reading results, so computations can run concurrently.
- A pipe carries one binary `unsigned long`. Parent and children share the same
  architecture, so no external serialization format is needed. Read and write
  helpers handle interrupted and partial transfers.
- The parent reads the results, calls `wait()` for every started child, checks
  exit statuses, and prints in argument order. Completion order may differ.
- Pipe/fork failures stop further creation, but existing children are still
  drained and reaped. Incomplete results and child failures cause a nonzero exit.

## Tests

From this directory, after compiling:

```sh
sh tests/check.sh
```

The portable suite checks the assignment example, base cases, eight arguments,
duplicates and order, numeric boundaries, leading zeroes, and invalid input.

Additional Linux-only checks require Python 3 and `strace`:

```sh
cc -std=c89 -Wall -Wextra -Werror -pedantic cpsc457-a1.c -o cpsc457-a1
python3 tests/check_linux.py
```

These verify all 48 supported indices against a separate reference, actual child
creation, pipe writes, parent-only stdout, child reaping, and cleanup after
injected pipe/fork failures. Python and strace are not needed to build or run the
assignment itself. See `VALIDATION.md` for the exact tested environments.

## Files and submission

- `cpsc457-a1.c`: complete assignment source.
- `README.md`: build and usage instructions.
- `reflection.pdf`: the requested technical reflection; review it before submission.
- `reflection.md`: editable text of that reflection.
- `tests/`: optional regression tests.
- `VALIDATION.md` and `validation/`: test evidence.

Repository: https://github.com/ElyasHossaini/Cpsc457

Submission is left to the student. No D2L submission is performed by this project.

## Course references

- [Assignment 1 specification](https://d2l.ucalgary.ca/d2l/le/content/769571/viewContent/7807879/View)
- [Minix Help Docs](https://d2l.ucalgary.ca/d2l/le/content/769571/viewContent/7805416/View)
- [Windows/Fedora shared-folder guide](https://d2l.ucalgary.ca/d2l/le/content/769571/viewContent/7815793/View)

The assignment explicitly requires Minix execution. The general assignment
policies also mention university Linux servers; this implementation was tested
on Ubuntu locally, not on the university servers.
