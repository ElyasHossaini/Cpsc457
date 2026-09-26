# CPSC 457 - A1: Reflection

The program limits process creation by validating all arguments before entering
a parent-only loop that calls fork once per accepted argument, with at most eight
arguments. Each child computes just its assigned Fibonacci number, sends the
result through its pipe, closes that descriptor, and calls _exit immediately;
this prevents a child from continuing the loop and creating extra processes.
The most demanding implementation detail is coordinating pipe ownership and
cleanup while keeping the computations parallel: waiting inside the creation
loop would serialize the work, while retaining unnecessary descriptors makes
end-of-file behavior harder to reason about. Each pipe is therefore created
before fork, so its descriptors are inherited by both processes. The child
closes its read end and inherited read ends from earlier pipes, and the parent
closes each write end immediately. All children are created before the parent
collects their fixed-size results. Transfer helpers handle partial operations
and interruptions, and the parent waits for every child even if later pipe or
process creation fails. Only the parent prints, in input order. The portable
regression suite exercises eight arguments, repeated indices, numeric boundaries,
and invalid input; additional Linux tracing verifies the number of children,
parent-only output, and cleanup under injected resource failures.
