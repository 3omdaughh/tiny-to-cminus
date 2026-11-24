#!/bin/sh
# run_tests.sh - regression tests for the C-Minus front end
#
# Run from the cminus/ directory (make test does this).
#
#   tests/scanner/{valid,invalid}/*.cm  run with  -s  (token listing)
#   tests/parser/{valid,invalid}/*.cm   run without   (syntax tree / errors)
#
# Each test X.cm has an expected output X.expected: the complete listing
# followed by a final line "exit=N" (0 = no errors, 1 = errors found).
# Every test is run with BOTH executables (Flex scanner and hand-written
# scanner); both must reproduce the expected output exactly.
#
#   tests/run_tests.sh            run all tests
#   tests/run_tests.sh --update   (re)write missing .expected files from ./cminus
#                                 -- review every new file by hand before committing!

EXECUTABLES="./cminus ./cminus_hand"
pass=0
fail=0

run_one() {   # $1 = executable, $2 = mode flag ("" or "-s"), $3 = test file
    $1 $2 "$3"
    echo "exit=$?"
}

for test in tests/scanner/*/*.cm tests/parser/*/*.cm; do
    case "$test" in
        tests/scanner/*) mode="-s" ;;
        *)               mode=""   ;;
    esac
    expected="${test%.cm}.expected"

    if [ "$1" = "--update" ]; then
        if [ ! -f "$expected" ]; then
            run_one ./cminus "$mode" "$test" > "$expected"
            echo "created  $expected"
        fi
        continue
    fi

    if [ ! -f "$expected" ]; then
        echo "MISSING  $expected"
        fail=$((fail + 1))
        continue
    fi

    for exe in $EXECUTABLES; do
        if run_one "$exe" "$mode" "$test" | diff -u "$expected" - > /dev/null; then
            pass=$((pass + 1))
        else
            fail=$((fail + 1))
            echo "FAIL     $exe $mode $test"
            run_one "$exe" "$mode" "$test" | diff -u "$expected" - | head -20
        fi
    done
done

[ "$1" = "--update" ] && exit 0

echo "----------------------------------------"
echo "passed: $pass   failed: $fail"
[ "$fail" -eq 0 ]
