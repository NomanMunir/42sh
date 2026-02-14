#!/bin/bash
# ============================================================================
# 42sh Test Runner
# Usage: ./tests/run_tests.sh [test_file]
#   No args   → runs all tests in tests/
#   With arg  → runs only the specified test file
# ============================================================================

SHELL_BIN="./42sh"
PASS=0
FAIL=0
TOTAL=0
FAILED_TESTS=""

# Colors
GREEN="\033[32m"
RED="\033[31m"
YELLOW="\033[33m"
CYAN="\033[36m"
BOLD="\033[1m"
RESET="\033[0m"

# --------------------------------------------------------------------------
# run_test "description" "command_to_test"
#   Runs the command in both bash and 42sh, compares stdout.
# --------------------------------------------------------------------------
run_test() {
    local desc="$1"
    local cmd="$2"
    TOTAL=$((TOTAL + 1))

    local expected
    local actual
    expected=$(echo "$cmd" | bash 2>/dev/null)
    actual=$(echo "$cmd" | $SHELL_BIN 2>/dev/null)

    if [ "$expected" = "$actual" ]; then
        printf "  ${GREEN}✓${RESET} %s\n" "$desc"
        PASS=$((PASS + 1))
    else
        printf "  ${RED}✗${RESET} %s\n" "$desc"
        printf "    ${YELLOW}expected:${RESET} %s\n" "$(echo "$expected" | head -3)"
        printf "    ${YELLOW}actual:  ${RESET} %s\n" "$(echo "$actual" | head -3)"
        FAIL=$((FAIL + 1))
        FAILED_TESTS="$FAILED_TESTS\n    - $desc"
    fi
}

# --------------------------------------------------------------------------
# run_test_exit "description" "command_to_test"
#   Compares exit codes between bash and 42sh.
# --------------------------------------------------------------------------
run_test_exit() {
    local desc="$1"
    local cmd="$2"
    TOTAL=$((TOTAL + 1))

    echo "$cmd" | bash 2>/dev/null
    local expected_exit=$?
    echo "$cmd" | $SHELL_BIN 2>/dev/null
    local actual_exit=$?

    if [ "$expected_exit" = "$actual_exit" ]; then
        printf "  ${GREEN}✓${RESET} %s (exit: %d)\n" "$desc" "$actual_exit"
        PASS=$((PASS + 1))
    else
        printf "  ${RED}✗${RESET} %s\n" "$desc"
        printf "    ${YELLOW}expected exit:${RESET} %d\n" "$expected_exit"
        printf "    ${YELLOW}actual exit:  ${RESET} %d\n" "$actual_exit"
        FAIL=$((FAIL + 1))
        FAILED_TESTS="$FAILED_TESTS\n    - $desc"
    fi
}

# --------------------------------------------------------------------------
# run_test_output "description" "command" "exact_expected_output"
#   Tests 42sh output against a hardcoded expected string (not bash).
# --------------------------------------------------------------------------
run_test_output() {
    local desc="$1"
    local cmd="$2"
    local expected="$3"
    TOTAL=$((TOTAL + 1))

    local actual
    actual=$(echo "$cmd" | $SHELL_BIN 2>/dev/null)

    if [ "$expected" = "$actual" ]; then
        printf "  ${GREEN}✓${RESET} %s\n" "$desc"
        PASS=$((PASS + 1))
    else
        printf "  ${RED}✗${RESET} %s\n" "$desc"
        printf "    ${YELLOW}expected:${RESET} %s\n" "$(echo "$expected" | head -3)"
        printf "    ${YELLOW}actual:  ${RESET} %s\n" "$(echo "$actual" | head -3)"
        FAIL=$((FAIL + 1))
        FAILED_TESTS="$FAILED_TESTS\n    - $desc"
    fi
}

# --------------------------------------------------------------------------
# section "Title"
# --------------------------------------------------------------------------
section() {
    printf "\n${BOLD}${CYAN}═══ %s ═══${RESET}\n" "$1"
}

# ============================================================================
# Check binary exists
# ============================================================================
if [ ! -x "$SHELL_BIN" ]; then
    printf "${RED}Error:${RESET} %s not found. Run 'make' first.\n" "$SHELL_BIN"
    exit 1
fi

# ============================================================================
# Run test files
# ============================================================================
TESTS_DIR="$(dirname "$0")"

if [ -n "$1" ]; then
    # Run a specific test file
    if [ -f "$1" ]; then
        source "$1"
    elif [ -f "$TESTS_DIR/$1" ]; then
        source "$TESTS_DIR/$1"
    else
        printf "${RED}Error:${RESET} Test file '%s' not found.\n" "$1"
        exit 1
    fi
else
    # Run all test files in order
    for test_file in "$TESTS_DIR"/test_*.sh; do
        [ -f "$test_file" ] && source "$test_file"
    done
fi

# ============================================================================
# Summary
# ============================================================================
printf "\n${BOLD}═══════════════════════════════════════${RESET}\n"
if [ $FAIL -eq 0 ]; then
    printf "${GREEN}${BOLD}All %d tests passed!${RESET}\n" "$TOTAL"
else
    printf "${GREEN}Passed:${RESET} %d / %d\n" "$PASS" "$TOTAL"
    printf "${RED}Failed:${RESET} %d / %d\n" "$FAIL" "$TOTAL"
    printf "\n${RED}Failed tests:${RESET}"
    printf "$FAILED_TESTS\n"
fi
printf "${BOLD}═══════════════════════════════════════${RESET}\n"

exit $FAIL
