#!/usr/bin/env python3
"""Additional Linux-only exhaustive, process-trace, and startup-failure checks."""
import pathlib
import re
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parent.parent
PROGRAM = str(ROOT / "cpsc457-a1")
ROW = re.compile(r"Child Process \(PID (\d+)\) F\{(\d+)\} = (\d+)")


def fib(n):
    a, b = 0, 1
    for _ in range(n):
        a, b = b, a + b
    return a


for first in range(0, 48, 8):
    indices = list(range(first, first + 8))
    result = subprocess.run([PROGRAM, *map(str, indices)], capture_output=True,
                            text=True, timeout=10, check=True)
    rows = [ROW.fullmatch(line) for line in result.stdout.splitlines()]
    assert len(rows) == len(indices) and all(rows), result.stdout
    assert not result.stderr, result.stderr
    assert len({r[1] for r in rows}) == len(indices)
    assert [(int(r[2]), int(r[3])) for r in rows] == [(n, fib(n)) for n in indices]
print("PASS all 48 supported Fibonacci indices against independent reference")

with tempfile.TemporaryDirectory() as folder:
    trace = pathlib.Path(folder) / "trace.log"

    def traced(arguments, injection=None):
        command = ["strace", "-f", "-o", str(trace), "-e", "trace=process,write,pipe,pipe2"]
        if injection:
            command += ["-e", injection]
        run = subprocess.run(command + [PROGRAM] + arguments, capture_output=True,
                             text=True, timeout=10)
        # strace may split one syscall across an unfinished/resumed pair.
        pending = {}
        lines = []
        for line in trace.read_text().splitlines():
            start = re.match(r"(\d+)\s+(\w+)\(.* <unfinished \.\.\.>$", line)
            resumed = re.match(r"(\d+)\s+<\.\.\. (\w+) resumed>(.*)", line)
            if start:
                pending[(start[1], start[2])] = line.split(" <unfinished")[0]
            elif resumed:
                lines.append(pending.pop((resumed[1], resumed[2])) + resumed[3])
            else:
                lines.append(line)
        return run, "\n".join(lines)

    run, log = traced([str(n) for n in range(8)])
    assert run.returncode == 0, run.stderr
    parent = re.search(r"^(\d+)\s+execve", log, re.M)[1]
    children = re.findall(r"^\d+\s+(?:clone|clone3|fork|vfork)\(.*= (\d+)\s*$", log, re.M)
    assert len(children) == 8 and len(set(children)) == 8, log
    writers = re.findall(r"^(\d+)\s+write\(1,", log, re.M)
    assert writers and set(writers) == {parent}, log
    for pid in children:
        descriptors = re.findall(r"^" + pid + r"\s+write\((\d+),", log, re.M)
        assert descriptors and all(int(fd) > 2 for fd in descriptors), log
    assert len(re.findall(r"^" + parent + r"\s+wait4\(.*= [1-9][0-9]*\s*$", log, re.M)) == 8, log
    print("PASS exactly 8 children, per-child pipe writes, parent-only stdout, 8 reaped children")

    run, log = traced(["3", "bad"])
    assert run.returncode != 0 and not run.stdout and run.stderr
    assert not re.search(r"\b(?:clone|clone3|fork|vfork)\(", log), log
    print("PASS invalid input creates no children")

    for syscall, error in [("clone", "EAGAIN"), ("pipe2", "EMFILE")]:
        run, log = traced(["3", "5", "9", "20"], f"inject={syscall}:error={error}:when=3")
        assert "INJECTED" in log, log
        assert run.returncode != 0 and not run.stdout and run.stderr, run
        reaped = re.findall(r"^\d+\s+wait4\(.*= ([1-9][0-9]*)\s*$", log, re.M)
        assert len(reaped) == 2 and len(set(reaped)) == 2, log
        print(f"PASS {syscall} failure after 2 children: diagnostic, cleanup, nonzero exit")
