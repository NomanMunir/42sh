#!/bin/bash
# ============================================================================
# Test: Edge Cases & Error Handling
# ============================================================================

section "Error Handling"

run_test "empty input" ""
run_test "spaces only" "   "
run_test "tab only" "	"
run_test "command not found" "nonexistent_cmd_42 2>/dev/null; echo \$?"

section "Complex Pipelines"

run_test "long pipe chain" "echo hello | cat | cat | cat | cat"
run_test "pipe + redirect" "echo abc | tr a-z A-Z"
run_test "seq + sort + head" "seq 5 1 | sort -n | head -3"

section "Mixed Features"

run_test "var in pipe" "X=hello; echo \$X | tr a-z A-Z"
run_test "semicolon + pipe" "echo first; echo second | tr a-z A-Z"
run_test "logic + pipe" "true && echo pass | tr a-z A-Z"
run_test "redirect + logic" \
    "TMP=\$(mktemp); echo ok > \$TMP && cat \$TMP; rm -f \$TMP"

section "Stress Tests"

run_test "many semicolons" \
    "echo 1; echo 2; echo 3; echo 4; echo 5; echo 6; echo 7; echo 8"
run_test "many pipes" \
    "echo test | cat | cat | cat | cat | cat | cat"
run_test "nested logic" \
    "true && true && true && false || echo recovered"
run_test "mixed operators" \
    "echo a; true && echo b; false || echo c; echo d"
