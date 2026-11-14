#!/bin/bash
# ============================================================================
# Test: Stability
# Hostile or broken input must never crash or hang the shell. These cases
# only check that 42sh ends by itself (see run_test_alive).
# ============================================================================

repeat() {
    local text="$1"
    local count="$2"
    local i=0
    while [ $i -lt "$count" ]; do
        printf '%s' "$text"
        i=$((i + 1))
    done
}

section "Runaway Recursion"

run_test_alive "function calling itself forever" 'f() { f; }; f; echo $?'
run_test_alive "recursion through command substitution" 'f() { echo $(f); }; f'
run_test_alive "eval evaluating itself" 'x="eval \$x"; eval $x'
run_test_alive "alias loop" "$(printf 'alias a=b b=a\na\nalias c="c c"\nc\n')"
run_test_alive "arithmetic variable loop" 'x=y; y=x; echo $((x)); z=z; echo $((z))'

section "Deep Nesting"

run_test_alive "3000 nested subshells" "$(repeat '(' 3000) echo x $(repeat ')' 3000)"
run_test_alive "3000 nested groups" "$(repeat '{ ' 3000) echo x; $(repeat '}; ' 3000)"
run_test_alive "2000 nested if" "$(repeat 'if true; then ' 2000) echo x; $(repeat 'fi; ' 2000)"
run_test_alive "200 nested command substitutions" "echo $(repeat '$(echo ' 200) x $(repeat ')' 200)"
run_test_alive "5000 nested arithmetic parentheses" "echo \$(( $(repeat '(' 5000) 1 $(repeat ')' 5000) ))"
run_test_alive "pipeline of 300 commands" "echo x $(repeat '| cat ' 300)"

section "Arithmetic Limits"

run_test_alive "division by zero" 'echo $((1 / 0)); echo $((1 % 0)); echo after'
run_test_alive "overflow" 'echo $((9223372036854775807 + 1)) $((-9223372036854775807 - 1))'
run_test_alive "most negative number divided by -1" \
    'echo $(( (-9223372036854775807 - 1) / -1 )) $(( (-9223372036854775807 - 1) % -1 ))'
run_test_alive "malformed expressions" 'echo $((1 +)) $((* 2)) $((1 2)) $(()) $((a b)) $((1 ? 2)) $((++)) $((x++ ++))'

section "Broken Syntax"

run_test_alive "nothing is closed" 'echo "abc $( ${ ` '"'"
run_test_alive "operators only" '; ;; & && || | ( ) { } < > >> << <& >& !'
run_test_alive "reserved words in disorder" 'fi done esac then do else elif in } )'
run_test_alive "unfinished compound commands" "$(printf 'if true\nfor x in\nwhile\ncase x in\nf() {\n')"
run_test_alive "here-document without end" "$(printf 'cat <<EOF\nnever closed\n')"
run_test_alive "strange parameter forms" \
    'echo ${} ${:} ${-} ${x:} ${#} ${##} ${#x#} ${x%%} ${!} ${1x} ${x/a/b} $[1+1]; echo after'
run_test_alive "strange assignments" '=x; 1x=2; x+=1; export 1x=2 =y; unset 1x =; echo after'
run_test_alive "strange redirections" \
    'echo x 99999999999>&1; echo z >&99; echo w <&-; cat <&77; echo > ; > > x; < ; echo a >& b; echo after'

section "Builtins with Bad Arguments"

run_test_alive "cd, exit, shift, break, return" \
    'cd a b c; cd -x; cd ""; shift 99; shift -1; shift x; break 0; break x; continue -1; return x; exit 1 2; echo $?'
run_test_alive "test and [" 'test -x; [ ; [ a b c d e ]; [ 1 -eq x ]; test ! ; test ! ! ! a; [ ] ]; test "(" ")"; echo $?'
run_test_alive "kill, wait, fg, bg, jobs" \
    'kill; kill -s; kill -l 999; kill -9999 1; kill %99; kill x; wait %9; wait x; wait -1; fg %9; bg %9; jobs %9; jobs -x; echo $?'
run_test_alive "fc and history" 'fc; fc -l; fc -s; fc -e; fc -l 1 2 3; fc -z; history; history -c; history x; echo $?'
run_test_alive "alias, hash, type, set, export, unset, read" \
    'hash -x; hash nosuch; type; type -x; unalias; unalias nosuch; alias =; alias =x; alias a==; export -x; export 1=2; unset -x; set -o nosuch; set +o; set -z; read 1x; read -x </dev/null; echo $?'
run_test_alive "eval, source" 'eval ")"; eval "((("; . ; . /; . /nonexistent; source /dev/null; echo $?'

section "Hostile Environment"

run_test_alive "standard output closed" 'exec >&-; echo hi; pwd; type echo; export -p; set; alias a=b; alias; hash; jobs; ls'
run_test_alive "standard error closed" 'exec 2>&-; nosuch; cd /nonexistent; echo ok; ls /nonexistent'
run_test_alive "all three closed" 'exec <&- >&- 2>&-; echo hi; read x; cat; ls'
run_test_alive "no space left on device" 'echo hi > /dev/full; pwd > /dev/full; export -p > /dev/full; echo $?'
run_test_alive "variables the shell relies on are unset" \
    'unset PATH HOME IFS PWD OLDPWD SHLVL PS1; ls; cd; cd -; echo ~; pwd; echo a b; type ls'
run_test_alive "current directory removed" \
    'mkdir -p /tmp/42sh_gone && cd /tmp/42sh_gone && rmdir /tmp/42sh_gone; pwd; cd .; ls; cd ..; pwd'
run_test_alive "one very long word" "echo $(repeat 'abcdefghij' 20000) | wc -c"
run_test_alive "200 background jobs" 'i=0; while [ $i -lt 200 ]; do true & i=$((i + 1)); done; wait; jobs; echo ok'
run_test_alive "process substitutions in a loop" \
    'i=0; while [ $i -lt 300 ]; do : <(:) >(:); i=$((i + 1)); done; echo ok'
