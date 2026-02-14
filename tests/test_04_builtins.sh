#!/bin/bash
# ============================================================================
# Test: Built-in Commands
# ============================================================================

section "echo"

run_test "echo basic" "echo hello"
run_test "echo -n flag" "echo -n no_newline"
run_test "echo multiple words" "echo one two three"
run_test "echo empty string" "echo ''"
run_test "echo no args" "echo"

section "cd & pwd"

run_test "cd to /tmp" "cd /tmp && pwd"
run_test "cd to / " "cd / && pwd"
run_test "cd home" "cd ~ && pwd"
run_test "cd dash (OLDPWD)" "cd /tmp; cd /; cd - 2>/dev/null; pwd"
run_test "cd nonexistent" "cd /nonexistent_42sh_dir 2>/dev/null; echo \$?"

section "type"

run_test_output "type echo" "type echo" "echo is a shell builtin"
run_test_output "type cd" "type cd" "cd is a shell builtin"
run_test_output "type exit" "type exit" "exit is a shell builtin"
run_test "type ls (external)" "type ls | head -1"

section "export & unset"

run_test "export and echo" "export MY_TEST_VAR=42sh_test; echo \$MY_TEST_VAR"
run_test "export with env" "export TESTXYZ=hello; env | grep TESTXYZ"
run_test "unset removes var" "export TESTXYZ=hello; unset TESTXYZ; echo \$TESTXYZ"

section "exit"

run_test_exit "exit 0" "exit 0"
run_test_exit "exit 42" "exit 42"
run_test_exit "exit no arg" "exit"
