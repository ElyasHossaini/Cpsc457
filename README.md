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

## Submission files

- `cpsc457-a1.c`: C source code.
- `README.md`: compilation and usage instructions.
- `reflection.pdf`: reflection report.

Repository: https://github.com/ElyasHossaini/Cpsc457
