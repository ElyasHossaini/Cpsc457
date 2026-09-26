#!/bin/sh
# Portable regression tests: run from the repository root after compiling.
set -u
program=./cpsc457-a1
scratch=tests/.check-$$
mkdir "$scratch" || exit 1
trap 'rm -f "$scratch/out" "$scratch/err" "$scratch/values" "$scratch/expected"; rmdir "$scratch"' 0
passed=0
failed=0

check_valid()
{
    name=$1
    expected=$2
    shift 2
    if "$program" "$@" >"$scratch/out" 2>"$scratch/err"; then
        sed 's/^Child Process (PID [0-9][0-9]*) //' "$scratch/out" >"$scratch/values"
        printf '%s\n' "$expected" >"$scratch/expected"
        unique=`sed 's/^Child Process (PID \([0-9][0-9]*\)).*/\1/' "$scratch/out" | sort -u | wc -l`
        if cmp "$scratch/values" "$scratch/expected" >/dev/null 2>&1 &&
           test ! -s "$scratch/err" && test "$unique" -eq "$#"; then
            echo "PASS $name"
            passed=`expr "$passed" + 1`
            return
        fi
    fi
    echo "FAIL $name"
    cat "$scratch/out" "$scratch/err"
    failed=`expr "$failed" + 1`
}

check_invalid()
{
    name=$1
    shift
    if "$program" "$@" >"$scratch/out" 2>"$scratch/err"; then
        echo "FAIL $name (accepted invalid input)"
        failed=`expr "$failed" + 1`
    elif test ! -s "$scratch/out" && test -s "$scratch/err"; then
        echo "PASS $name"
        passed=`expr "$passed" + 1`
    else
        echo "FAIL $name (wrong diagnostic/output)"
        failed=`expr "$failed" + 1`
    fi
}

check_valid assignment_sample 'F{3} = 2
F{5} = 5
F{2} = 1
F{9} = 34
F{20} = 6765' 3 5 2 9 20
check_valid base_cases 'F{0} = 0
F{1} = 1
F{2} = 1' 0 1 2
check_valid eight_arguments 'F{0} = 0
F{1} = 1
F{2} = 1
F{3} = 2
F{4} = 3
F{5} = 5
F{6} = 8
F{7} = 13' 0 1 2 3 4 5 6 7
check_valid repeats_and_order 'F{9} = 34
F{3} = 2
F{9} = 34
F{0} = 0' 9 3 9 0
check_valid upper_boundary 'F{46} = 1836311903
F{47} = 2971215073' 46 47
check_valid leading_zeroes 'F{3} = 2
F{0} = 0' 0003 0000
check_invalid no_arguments
check_invalid nine_arguments 0 1 2 3 4 5 6 7 8
check_invalid negative -1
check_invalid too_large 48
check_invalid integer_overflow 99999999999999999999999999999999999
check_invalid text abc
check_invalid mixed_digits 2x
check_invalid empty ''
check_invalid whitespace ' 3'
check_invalid decimal 3.5
check_invalid plus_sign +3
check_invalid later_bad_argument 3 5 nope
echo "RESULT: $passed passed; $failed failed"
test "$failed" -eq 0
