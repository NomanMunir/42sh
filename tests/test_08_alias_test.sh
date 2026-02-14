#!/bin/bash
# ============================================================================
# Test: Aliases, History, Test Builtin
# ============================================================================

section "Aliases"

run_test_output "alias define and use" \
    'alias greet="echo hello"; greet' \
    "hello"

run_test_output "alias with args" \
    'alias say="echo"; say world' \
    "world"

run_test_output "unalias removes" \
    'alias hi="echo hi"; unalias hi; hi 2>/dev/null; echo $?' \
    "127"

section "Test Builtin"

# File tests
run_test_output "test -f on file" \
    "test -f /etc/hostname; echo \$?" \
    "0"

run_test_output "test -f on dir" \
    "test -f /tmp; echo \$?" \
    "1"

run_test_output "test -d on dir" \
    "test -d /tmp; echo \$?" \
    "0"

run_test_output "test -e exists" \
    "test -e /etc/hostname; echo \$?" \
    "0"

run_test_output "test -e nonexistent" \
    "test -e /nonexistent_42sh; echo \$?" \
    "1"

# String tests
run_test_output "test string equal" \
    'test "abc" = "abc"; echo $?' \
    "0"

run_test_output "test string not equal" \
    'test "abc" != "xyz"; echo $?' \
    "0"

run_test_output "test -z empty" \
    'test -z ""; echo $?' \
    "0"

run_test_output "test -n non-empty" \
    'test -n "hello"; echo $?' \
    "0"

# Integer tests
run_test_output "test 1 -eq 1" \
    "test 1 -eq 1; echo \$?" \
    "0"

run_test_output "test 1 -ne 2" \
    "test 1 -ne 2; echo \$?" \
    "0"

run_test_output "test 5 -gt 3" \
    "test 5 -gt 3; echo \$?" \
    "0"

run_test_output "test 3 -lt 5" \
    "test 3 -lt 5; echo \$?" \
    "0"

# Bracket syntax
run_test_output "bracket -d /tmp" \
    '[ -d /tmp ]; echo $?' \
    "0"

# Negation
run_test_output "test negation" \
    "test ! -f /nonexistent_42sh; echo \$?" \
    "0"

section "Test + Logical Operators"

run_test_output "test && echo" \
    "test -d /tmp && echo exists" \
    "exists"

run_test_output "test || fallback" \
    "test -f /nonexistent_42sh || echo missing" \
    "missing"
