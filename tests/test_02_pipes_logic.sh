#!/bin/bash
# ============================================================================
# Test: Pipes, Semicolons, Logical Operators
# ============================================================================

section "Pipes"

run_test "simple pipe" "echo hello | cat"
run_test "pipe to grep" "echo -e 'foo\nbar\nbaz' | grep bar"
run_test "pipe to wc -l" "echo -e 'a\nb\nc' | wc -l"
run_test "multi pipe" "echo hello world | tr ' ' '\n' | sort"
run_test "pipe to head" "seq 1 100 | head -5"
run_test "pipe to tail" "seq 1 10 | tail -3"
run_test "pipe exit code is right side" "false | true; echo \$?"
run_test "pipe with cat" "echo test123 | cat | cat | cat"

section "Semicolons"

run_test "two commands" "echo first; echo second"
run_test "three commands" "echo 1; echo 2; echo 3"
run_test "semicolons with spaces" "echo a;echo b;echo c"
run_test "trailing semicolon" "echo done;"

section "Logical Operators"

run_test "AND success" "true && echo yes"
run_test "AND failure" "false && echo no"
run_test "OR success" "true || echo no"
run_test "OR failure" "false || echo fallback"
run_test "chain AND" "true && true && echo all_pass"
run_test "chain OR" "false || false || echo final"
run_test "AND then OR" "true && false || echo recovered"
run_test "complex logic" "true && echo a; false || echo b"
run_test "AND short circuit" "false && echo never; echo after"
