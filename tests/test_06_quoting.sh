#!/bin/bash
# ============================================================================
# Test: Quoting / Inhibitors
# ============================================================================

section "Single Quotes"

run_test "single quotes literal" "echo 'hello world'"
run_test "single quotes no expansion" "X=test; echo '\$X'"
run_test "single quotes preserve spaces" "echo 'a   b   c'"
run_test "single empty" "echo ''"
run_test "concat with single" "echo 'hel''lo'"

section "Double Quotes"

run_test "double quotes" 'echo "hello world"'
run_test "double quotes expand var" 'X=test; echo "$X"'
run_test "double quotes preserve spaces" 'echo "a   b   c"'
run_test "double empty" 'echo ""'
run_test "double with exit code" 'true; echo "exit: $?"'

section "Backslash Escaping"

run_test "escape space" "echo hello\\ world"
run_test "escape dollar" 'echo \$HOME'
run_test "escape in double quotes" 'echo "hello\"world"'
run_test "escape backslash" 'echo "back\\\\slash"'

section "Mixed Quoting"

run_test "single then double" "echo 'single'\"double\""
run_test "quote in word" "echo he\"ll\"o"
run_test "var only in double" 'X=yes; echo "$X" '"'"'$X'"'"''
