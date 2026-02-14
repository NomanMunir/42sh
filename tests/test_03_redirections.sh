#!/bin/bash
# ============================================================================
# Test: Redirections
# ============================================================================

section "Output Redirections"

TMP_DIR=$(mktemp -d)

run_test "redirect stdout to file" \
    "echo hello > $TMP_DIR/out1.txt && cat $TMP_DIR/out1.txt"

run_test "redirect overwrites file" \
    "echo first > $TMP_DIR/out2.txt; echo second > $TMP_DIR/out2.txt; cat $TMP_DIR/out2.txt"

run_test "append redirect" \
    "echo line1 > $TMP_DIR/app.txt; echo line2 >> $TMP_DIR/app.txt; cat $TMP_DIR/app.txt"

run_test "redirect with pipe" \
    "echo pipe_test > $TMP_DIR/pipe.txt; cat $TMP_DIR/pipe.txt | tr a-z A-Z"

section "Input Redirections"

echo "input_content" > "$TMP_DIR/input.txt"

run_test "read from file" \
    "cat < $TMP_DIR/input.txt"

run_test "input + output redirect" \
    "cat < $TMP_DIR/input.txt > $TMP_DIR/copy.txt; cat $TMP_DIR/copy.txt"

section "Redirect Edge Cases"

run_test "redirect nonexistent file (input)" \
    "cat < /nonexistent_42sh_file 2>/dev/null; echo \$?"

run_test "multiple redirects" \
    "echo multi > $TMP_DIR/m1.txt > $TMP_DIR/m2.txt; cat $TMP_DIR/m2.txt"

# Cleanup
rm -rf "$TMP_DIR"
