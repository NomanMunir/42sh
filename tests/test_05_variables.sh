#!/bin/bash
# ============================================================================
# Test: Variables & Expansion
# ============================================================================

section "Variable Assignment & Expansion"

run_test "simple assignment" "X=hello; echo \$X"
run_test "braces syntax" "X=world; echo \${X}"
run_test "exit code var" "true; echo \$?"
run_test "exit code after false" "false; echo \$?"
run_test "multiple assignments" "A=1; B=2; echo \$A \$B"
run_test "var in middle of word" 'X=hello; echo prefix${X}suffix'
run_test "undefined var is empty" "echo \$UNDEFINED_42SH_VAR"
run_test "var overwrite" "X=first; X=second; echo \$X"

section "Environment Variables"

run_test "HOME is set" "echo \$HOME"
run_test "PATH is set" "echo \$PATH | head -c 5"
run_test "USER is set" "echo \$USER"
run_test "PWD matches pwd" 'echo $PWD'
run_test "export promotes var" "X=internal; export X; env | grep ^X="
