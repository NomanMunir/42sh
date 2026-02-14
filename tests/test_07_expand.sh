#!/bin/bash
# ============================================================================
# Test: Tilde, Globbing, Command Substitution
# ============================================================================

section "Tilde Expansion"

run_test "bare tilde" "echo ~"
run_test "tilde slash" 'echo ~/foobar'
run_test "cd tilde" "cd ~ && pwd"

section "Globbing"

# Create test environment
TMP_DIR=$(mktemp -d)
touch "$TMP_DIR/foo.c" "$TMP_DIR/bar.c" "$TMP_DIR/baz.h" "$TMP_DIR/readme.md"

run_test "star glob" "cd $TMP_DIR && echo *.c"
run_test "question mark glob" "cd $TMP_DIR && echo ba?.c"
run_test "bracket glob" "cd $TMP_DIR && echo [bf]*.c"
run_test "no match keeps literal" "cd $TMP_DIR && echo *.xyz"
run_test "star matches all" "cd $TMP_DIR && echo * | tr ' ' '\n' | wc -l"

rm -rf "$TMP_DIR"

section "Command Substitution"

run_test "simple cmd sub" 'echo $(echo hello)'
run_test "cmd sub in sentence" 'echo "result: $(echo 42)"'
run_test "cmd sub with pipe" 'echo $(echo a; echo b; echo c)'
run_test "nested cmd sub" 'echo $(echo $(echo deep))'
run_test "cmd sub exit code" '$(true); echo $?'
run_test "cmd sub preserves value" 'X=$(echo captured); echo $X'
