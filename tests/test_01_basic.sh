#!/bin/bash
# ============================================================================
# Test: Basic Commands & PATH Resolution
# ============================================================================

section "Basic Commands"

run_test "echo simple string" "echo hello"
run_test "echo multiple args" "echo hello world foo bar"
run_test "echo empty" "echo"
run_test "ls root" "ls /"
run_test "pwd" "pwd"
run_test "whoami" "whoami"
run_test "uname" "uname"
run_test "true" "true"
run_test "false; echo \$?" "false; echo \$?"
run_test "/bin/echo absolute path" "/bin/echo absolute"
run_test "cat /etc/hostname" "cat /etc/hostname"

section "Exit Codes"

run_test_exit "true exits 0" "true"
run_test_exit "false exits 1" "false"
run_test "echo exit code of true" "true; echo \$?"
run_test "echo exit code of false" "false; echo \$?"
run_test "command not found exit code" "nonexistent_cmd_42sh; echo \$?"
