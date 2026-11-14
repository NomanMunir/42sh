#!/bin/bash
# ============================================================================
# Test: Mandatory and modular features, one section per feature
# Every case is run by bash and by 42sh; their standard outputs must match.
# ============================================================================

section "Redirection and Aggregation"

run_test 'ls doesnotexist . 2>&1 >/dev/null | sed s/^ls: // | cut...' "$(cat <<'__CASE__'
ls doesnotexist . 2>&1 >/dev/null | sed 's/^ls: //' | cut -c1-30
__CASE__
)"

run_test 'ls doesnotexist . >/dev/null 2>&1; echo $?' "$(cat <<'__CASE__'
ls doesnotexist . >/dev/null 2>&1; echo $?
__CASE__
)"

run_test 'echo out >&2 2>&1 | cat; echo err 2>&1 >&2 | cat' "$(cat <<'__CASE__'
echo out >&2 2>&1 | cat; echo err 2>&1 >&2 | cat
__CASE__
)"

run_test 'cat <&- 2>/dev/null; echo $?' "$(cat <<'__CASE__'
cat <&- 2>/dev/null; echo $?
__CASE__
)"

run_test 'echo hidden >&-; echo $?' "$(cat <<'__CASE__'
echo hidden >&-; echo $?
__CASE__
)"

run_test 'echo a 1>/tmp/42sh_f1 2>&1; cat /tmp/42sh_f1; rm -f /tmp/...' "$(cat <<'__CASE__'
echo a 1>/tmp/42sh_f1 2>&1; cat /tmp/42sh_f1; rm -f /tmp/42sh_f1
__CASE__
)"

run_test 'exec 4</etc/hostname; cat <&4 | wc -l; exec 4<&-' "$(cat <<'__CASE__'
exec 4</etc/hostname; cat <&4 | wc -l; exec 4<&-
__CASE__
)"

run_test 'echo abc > /tmp/42sh_f2; cat < /tmp/42sh_f2 > /tmp/42sh_f...' "$(cat <<'__CASE__'
echo abc > /tmp/42sh_f2; cat < /tmp/42sh_f2 > /tmp/42sh_f3; cat /tmp/42sh_f3; wc -c < /tmp/42sh_f3; rm -f /tmp/42sh_f2 /tmp/42sh_f3
__CASE__
)"

run_test 'cat << A << B' "$(cat <<'__CASE__'
cat << A << B
first
A
second
B
__CASE__
)"

run_test 'cat <<EOF | tr a-z A-Z' "$(cat <<'__CASE__'
cat <<EOF | tr a-z A-Z
piped heredoc
EOF
__CASE__
)"

run_test 'x=val; cat <<EOF' "$(cat <<'__CASE__'
x=val; cat <<EOF
$x ${x} $(echo cmd) $((1+2)) \$x "quoted" 'single'
EOF
__CASE__
)"

run_test 'echo one > /tmp/42sh_f4; echo two >| /tmp/42sh_f4; cat /t...' "$(cat <<'__CASE__'
echo one > /tmp/42sh_f4; echo two >| /tmp/42sh_f4; cat /tmp/42sh_f4; echo three 1<>/tmp/42sh_f4; cat /tmp/42sh_f4; rm /tmp/42sh_f4
__CASE__
)"

run_test '{ echo a; echo b >&2; } 2>&1 | sort' "$(cat <<'__CASE__'
{ echo a; echo b >&2; } 2>&1 | sort
__CASE__
)"

section "Subshells and Groups"

run_test '(echo sub; exit 4); echo $?' "$(cat <<'__CASE__'
(echo sub; exit 4); echo $?
__CASE__
)"

run_test '( ( ( echo deep ) ) )' "$(cat <<'__CASE__'
( ( ( echo deep ) ) )
__CASE__
)"

run_test '{ { echo grouped; }; }' "$(cat <<'__CASE__'
{ { echo grouped; }; }
__CASE__
)"

run_test '(cd /tmp; x=inner; echo $x); echo "[$x]"; pwd | grep -c tmp' "$(cat <<'__CASE__'
(cd /tmp; x=inner; echo $x); echo "[$x]"; pwd | grep -c tmp
__CASE__
)"

run_test 'x=outer; { x=changed; }; echo $x' "$(cat <<'__CASE__'
x=outer; { x=changed; }; echo $x
__CASE__
)"

run_test 'echo a; { echo b; echo c; } > /tmp/42sh_f5; cat /tmp/42sh...' "$(cat <<'__CASE__'
echo a; { echo b; echo c; } > /tmp/42sh_f5; cat /tmp/42sh_f5; rm /tmp/42sh_f5
__CASE__
)"

run_test '(echo a; echo b) | wc -l' "$(cat <<'__CASE__'
(echo a; echo b) | wc -l
__CASE__
)"

section "Parameter Formats"

run_test 'x=abc; echo ${x} ${x:-def} ${y:-def} ${y:=set} $y ${x:+al...' "$(cat <<'__CASE__'
x=abc; echo ${x} ${x:-def} ${y:-def} ${y:=set} $y ${x:+alt} ${z:+alt} ${#x} ${#z}
__CASE__
)"

run_test 'y=; echo "[${y:-null}]" "[${y-null}]" "[${y:+alt}]" "[${y...' "$(cat <<'__CASE__'
y=; echo "[${y:-null}]" "[${y-null}]" "[${y:+alt}]" "[${y+alt}]" "[${u-unset}]" "[${u+alt}]"
__CASE__
)"

run_test ': ${v:?} 2>/dev/null; echo no' "$(cat <<'__CASE__'
: ${v:?} 2>/dev/null; echo no
__CASE__
)"

run_test 'f=/usr/local/lib/libfoo.so.1.2; echo ${f%.*} ${f%%.*} ${f...' "$(cat <<'__CASE__'
f=/usr/local/lib/libfoo.so.1.2; echo ${f%.*} ${f%%.*} ${f#*/} ${f##*/} ${f%/*} ${f##*.}
__CASE__
)"

run_test 'set -- one two three; echo ${#} ${#1} ${2:-x} ${4:-none} ...' "$(cat <<'__CASE__'
set -- one two three; echo ${#} ${#1} ${2:-x} ${4:-none} "${*}" ${#@}
__CASE__
)"

section "Arithmetic Expansion"

run_test 'echo $((2 + 3 * 4)) $(((2 + 3) * 4)) $((10 / 3)) $((10 % ...' "$(cat <<'__CASE__'
echo $((2 + 3 * 4)) $(((2 + 3) * 4)) $((10 / 3)) $((10 % 3)) $((-7 / 2)) $((2 - 5))
__CASE__
)"

run_test 'echo $((3 <= 3)) $((3 >= 4)) $((3 < 4)) $((3 > 4)) $((3 =...' "$(cat <<'__CASE__'
echo $((3 <= 3)) $((3 >= 4)) $((3 < 4)) $((3 > 4)) $((3 == 3)) $((3 != 3))
__CASE__
)"

run_test 'echo $((1 && 0)) $((1 || 0)) $((0 || 0)) $((1 && 1)) $((!...' "$(cat <<'__CASE__'
echo $((1 && 0)) $((1 || 0)) $((0 || 0)) $((1 && 1)) $((!1)) $((!0))
__CASE__
)"

run_test 'i=5; echo $((i++)) $i $((++i)) $i $((i--)) $i $((--i)) $i' "$(cat <<'__CASE__'
i=5; echo $((i++)) $i $((++i)) $i $((i--)) $i $((--i)) $i
__CASE__
)"

run_test 'a=3; b=4; echo $((a * b + a)) $(( a > b || b > a )) $((a+...' "$(cat <<'__CASE__'
a=3; b=4; echo $((a * b + a)) $(( a > b || b > a )) $((a+b==7 && a<b))
__CASE__
)"

run_test 'echo $((  1  +  1  )) $((1+1))$((2+2))' "$(cat <<'__CASE__'
echo $((  1  +  1  )) $((1+1))$((2+2))
__CASE__
)"

run_test 'echo $(( $(echo 3) + ${n:-4} ))' "$(cat <<'__CASE__'
echo $(( $(echo 3) + ${n:-4} ))
__CASE__
)"

section "Process Substitution"

run_test 'cat <(echo one) <(echo two)' "$(cat <<'__CASE__'
cat <(echo one) <(echo two)
__CASE__
)"

run_test 'diff <(echo same) <(echo same); echo $?' "$(cat <<'__CASE__'
diff <(echo same) <(echo same); echo $?
__CASE__
)"

run_test 'echo data > >(cat); sleep 0.2' "$(cat <<'__CASE__'
echo data > >(cat); sleep 0.2
__CASE__
)"

run_test 'wc -l < <(printf anbn)' "$(cat <<'__CASE__'
wc -l < <(printf 'a\nb\n')
__CASE__
)"

section "Tilde Expansion"

run_test 'echo ~ ~/x ~root ~nonexistentuser_zz "~" a~ | sed "s|$HOM...' "$(cat <<'__CASE__'
echo ~ ~/x ~root ~nonexistentuser_zz "~" a~ | sed "s|$HOME|HOME|g"
__CASE__
)"

run_test 'x=~; y=~/bin:~/lib; echo $x $y | sed "s|$HOME|HOME|g"' "$(cat <<'__CASE__'
x=~; y=~/bin:~/lib; echo $x $y | sed "s|$HOME|HOME|g"
__CASE__
)"

section "Inhibitors"

run_test 'echo "a b" c d e f "gh" i"j "k"l" mn "no"  ...' "$(cat <<'__CASE__'
echo "a b" 'c d' e\ f "g'h" 'i"j' "k\"l" 'm\n' "n\\o" \\ \" \'
__CASE__
)"

run_test 'echo "$HOME" $HOME $HOME "$HOME" | sed "s|$HOME|HOME|g"' "$(cat <<'__CASE__'
echo "$HOME" '$HOME' \$HOME "\$HOME" | sed "s|$HOME|HOME|g"
__CASE__
)"

run_test 'echo "a' "$(cat <<'__CASE__'
echo "a
b" 'c
d'
__CASE__
)"

run_test 'x=a b; echo $x "$x" $x "$x" "$x"' "$(cat <<'__CASE__'
x='a b'; echo $x "$x" '$x' "'$x'" '"$x"'
__CASE__
)"

run_test 'echo abc "abc" abc' "$(cat <<'__CASE__'
echo \a\b\c "\a\b\c" '\a\b\c'
__CASE__
)"

section "Pattern Matching"

run_test 'mkdir -p /tmp/42sh_g && cd /tmp/42sh_g && touch a1 a2 b1 ...' "$(cat <<'__CASE__'
mkdir -p /tmp/42sh_g && cd /tmp/42sh_g && touch a1 a2 b1 .hid 'sp ace' && echo * && echo a? && echo [ab]1 && echo [!a]* && echo [a-b]? && echo .* | tr ' ' '\n' | grep -c hid; echo "*" '*' \* 'a'*; cd /; rm -rf /tmp/42sh_g
__CASE__
)"

run_test 'mkdir -p /tmp/42sh_h/d1/d2 && cd /tmp/42sh_h && touch d1/...' "$(cat <<'__CASE__'
mkdir -p /tmp/42sh_h/d1/d2 && cd /tmp/42sh_h && touch d1/f1 d1/d2/f2 && echo */* && echo */*/* && echo d1/f[0-9] && echo nomatch*; cd /; rm -rf /tmp/42sh_h
__CASE__
)"

section "Builtin test"

run_test 'test -d /tmp && test -e /etc/passwd && test -f /etc/passw...' "$(cat <<'__CASE__'
test -d /tmp && test -e /etc/passwd && test -f /etc/passwd && test -r /etc/passwd && test -s /etc/passwd && test -x /bin/sh && test -w /tmp && echo files-ok
__CASE__
)"

run_test 'test -L /bin/sh; a=$?; test -b /dev/null; b=$?; test -c /...' "$(cat <<'__CASE__'
test -L /bin/sh; a=$?; test -b /dev/null; b=$?; test -c /dev/null; c=$?; test -p /dev/null; d=$?; test -S /dev/null; e=$?; test -g /tmp; f=$?; test -u /usr/bin/su; g=$?; echo $a $b $c $d $e $f $g
__CASE__
)"

run_test 'test abc; echo $?; test ""; echo $?; test; echo $?; test ...' "$(cat <<'__CASE__'
test abc; echo $?; test ""; echo $?; test; echo $?; test -z ""; echo $?; test -z x; echo $?
__CASE__
)"

run_test 'test a = a; echo $?; test a != a; echo $?; test 5 -eq 5; ...' "$(cat <<'__CASE__'
test a = a; echo $?; test a != a; echo $?; test 5 -eq 5; echo $?; test 5 -ne 5; echo $?; test 5 -ge 5; echo $?; test 4 -lt 5; echo $?; test 6 -le 5; echo $?
__CASE__
)"

run_test 'test ! -d /nonexistent; echo $?; test ! a = a; echo $?; [...' "$(cat <<'__CASE__'
test ! -d /nonexistent; echo $?; test ! a = a; echo $?; [ ! -z x ]; echo $?; [ ! ]; echo $?
__CASE__
)"

run_test '[ -d /tmp; echo $?' "$(cat <<'__CASE__'
[ -d /tmp; echo $?
__CASE__
)"

section "Aliases"

run_test 'alias e=echo aliased; e one' "$(cat <<'__CASE__'
alias e='echo aliased'; e one
__CASE__
)"

run_test 'alias a1=echo ; alias a2=chained; a1 a2' "$(cat <<'__CASE__'
alias a1='echo '; alias a2='chained'; a1 a2
__CASE__
)"

run_test 'alias ls=ls -d; alias ls 2>&1 | head -1; unalias ls; al...' "$(cat <<'__CASE__'
alias ls='ls -d'; alias ls 2>&1 | head -1; unalias ls; alias ls 2>/dev/null; echo $?
__CASE__
)"

run_test 'alias x=y; unalias -a; alias; echo $?' "$(cat <<'__CASE__'
alias x=y; unalias -a; alias; echo $?
__CASE__
)"

section "Hash Table and type"

run_test 'hash -r; hash; hash ls cat; hash | wc -l; ls /dev/null; h...' "$(cat <<'__CASE__'
hash -r; hash; hash ls cat; hash | wc -l; ls /dev/null; hash | grep -c ls
__CASE__
)"

run_test 'type type; type ls | sed s|/usr||; type nonexistent_zz ...' "$(cat <<'__CASE__'
type type; type ls | sed 's|/usr||'; type nonexistent_zz 2>&1 | grep -c 'not found'; type if
__CASE__
)"

section "Variables"

run_test 'a=1 b=2; export a; env | grep -E ^[ab]= ; unset a b; ec...' "$(cat <<'__CASE__'
a=1 b=2; export a; env | grep -E '^[ab]=' ; unset a b; echo "[$a$b]"
__CASE__
)"

run_test 'set | grep -c ^HOME=; x=5; set | grep ^x=' "$(cat <<'__CASE__'
set | grep -c '^HOME='; x=5; set | grep '^x='
__CASE__
)"

section "Builtins cd, echo, exit"

run_test 'cd /tmp; echo $PWD $OLDPWD | grep -c tmp; cd /; cd -; ech...' "$(cat <<'__CASE__'
cd /tmp; echo $PWD $OLDPWD | grep -c tmp; cd /; cd -; echo $OLDPWD
__CASE__
)"

run_test 'cd /tmp/../tmp/./; pwd; cd //; pwd | grep -c /' "$(cat <<'__CASE__'
cd /tmp/../tmp/./; pwd; cd //; pwd | grep -c /
__CASE__
)"

run_test 'CDPATH=/usr; cd lib >/dev/null; pwd' "$(cat <<'__CASE__'
CDPATH=/usr; cd lib >/dev/null; pwd
__CASE__
)"

run_test 'echo; echo -n; echo -n x; echo; echo -- -n; echo - ; echo...' "$(cat <<'__CASE__'
echo; echo -n; echo -n x; echo; echo -- -n; echo - ; echo -nn y; echo; echo -x z
__CASE__
)"

run_test '(exit 0); echo $?; (exit 255); echo $?; (exit 256); echo $?' "$(cat <<'__CASE__'
(exit 0); echo $?; (exit 255); echo $?; (exit 256); echo $?
__CASE__
)"

section "Logical Operators and Jobs"

run_test 'true; false || true && echo chain; false && true || echo ...' "$(cat <<'__CASE__'
true; false || true && echo chain; false && true || echo chain2
__CASE__
)"

run_test 'echo a & wait; echo b' "$(cat <<'__CASE__'
echo a & wait; echo b
__CASE__
)"

run_test 'sleep 0.2 & sleep 0.1 & wait; echo both' "$(cat <<'__CASE__'
sleep 0.2 & sleep 0.1 & wait; echo both
__CASE__
)"

run_test '(sleep 0.1; exit 9) & p=$!; wait $p; echo $?' "$(cat <<'__CASE__'
(sleep 0.1; exit 9) & p=$!; wait $p; echo $?
__CASE__
)"

run_test 'false & wait $!; echo $?; true & wait $!; echo $?' "$(cat <<'__CASE__'
false & wait $!; echo $?; true & wait $!; echo $?
__CASE__
)"

run_test 'jobs; sleep 0.1 & wait; jobs' "$(cat <<'__CASE__'
jobs; sleep 0.1 & wait; jobs
__CASE__
)"

section "Command Substitution"

run_test 'x=$(echo a; echo b); echo "$x"; y=`echo c`; echo $y' "$(cat <<'__CASE__'
x=$(echo a; echo b); echo "$x"; y=`echo c`; echo $y
__CASE__
)"

run_test 'echo "$(echo "inner $(echo "deeper $(echo deepest)")")"' "$(cat <<'__CASE__'
echo "$(echo "inner $(echo "deeper $(echo deepest)")")"
__CASE__
)"

run_test 'echo $(echo $(echo $(echo nested)))' "$(cat <<'__CASE__'
echo $(echo $(echo $(echo nested)))
__CASE__
)"

section "Field Splitting"

run_test 'v="a   b   c"; echo $v; echo "$v"; set -- $v; echo $#' "$(cat <<'__CASE__'
v="a   b   c"; echo $v; echo "$v"; set -- $v; echo $#
__CASE__
)"

run_test 'IFS=,; v="a,b,,c"; set -- $v; echo $#; IFS= ; echo "$1|...' "$(cat <<'__CASE__'
IFS=,; v="a,b,,c"; set -- $v; echo $#; IFS=' '; echo "$1|$2|$3|$4"
__CASE__
)"

run_test 'unset IFS; v="  a  b  "; set -- $v; echo $# "[$1]"' "$(cat <<'__CASE__'
unset IFS; v="  a  b  "; set -- $v; echo $# "[$1]"
__CASE__
)"

run_test 'e=; echo [$e] ["$e"]; set -- $e; echo $#; set -- "$e"; ec...' "$(cat <<'__CASE__'
e=; echo [$e] ["$e"]; set -- $e; echo $#; set -- "$e"; echo $#
__CASE__
)"

section "Functions"

run_test 'f() { echo "$#:$1:$2"; }; f; f a; f "a b" c; f "" ""; f $...' "$(cat <<'__CASE__'
f() { echo "$#:$1:$2"; }; f; f a; f "a b" c; f "" ""; f $nothing; f "$nothing"
__CASE__
)"

run_test 'f() { return 5; }; f; echo $?; f() { false; }; f; echo $?...' "$(cat <<'__CASE__'
f() { return 5; }; f; echo $?; f() { false; }; f; echo $?; f() { :; }; f; echo $?
__CASE__
)"

run_test 'f() { g() { echo inner; }; }; f; g' "$(cat <<'__CASE__'
f() { g() { echo inner; }; }; f; g
__CASE__
)"

run_test 'fact() { if [ $1 -le 1 ]; then echo 1; else echo $(( $1 *...' "$(cat <<'__CASE__'
fact() { if [ $1 -le 1 ]; then echo 1; else echo $(( $1 * $(fact $(($1 - 1))) )); fi; }; fact 10
__CASE__
)"

section "Loops, case, if"

run_test 'for x in; do echo never; done; echo $?; for x in a; do :;...' "$(cat <<'__CASE__'
for x in; do echo never; done; echo $?; for x in a; do :; done; echo $x
__CASE__
)"

run_test 'for x; do echo $x; done; set -- p q; for x; do echo $x; done' "$(cat <<'__CASE__'
for x; do echo $x; done; set -- p q; for x; do echo $x; done
__CASE__
)"

run_test 'i=0; while true; do i=$((i+1)); [ $i -ge 3 ] && break; do...' "$(cat <<'__CASE__'
i=0; while true; do i=$((i+1)); [ $i -ge 3 ] && break; done; echo $i
__CASE__
)"

run_test 'i=0; until [ $i -ge 3 ]; do i=$((i+1)); done; echo $i' "$(cat <<'__CASE__'
i=0; until [ $i -ge 3 ]; do i=$((i+1)); done; echo $i
__CASE__
)"

run_test 'for i in 1 2; do for j in 1 2; do [ $j = 2 ] && break 2; ...' "$(cat <<'__CASE__'
for i in 1 2; do for j in 1 2; do [ $j = 2 ] && break 2; echo $i$j; done; done; echo out
__CASE__
)"

run_test 'case $HOME in /*) echo abs ;; *) echo rel ;; esac; case x...' "$(cat <<'__CASE__'
case $HOME in /*) echo abs ;; *) echo rel ;; esac; case x in (x) echo paren ;; esac; case y in x) ;; esac; echo $?
__CASE__
)"

run_test 'case "a b" in "a b") echo quoted ;; esac; case * in *...' "$(cat <<'__CASE__'
case "a b" in "a b") echo quoted ;; esac; case '*' in '*') echo lit ;; esac; case abc in 'a*') echo no ;; a*) echo glob ;; esac
__CASE__
)"

run_test 'if false; then :; fi; echo $?; if true; then false; fi; e...' "$(cat <<'__CASE__'
if false; then :; fi; echo $?; if true; then false; fi; echo $?
__CASE__
)"

run_test 'while false; do :; done; echo $?' "$(cat <<'__CASE__'
while false; do :; done; echo $?
__CASE__
)"

run_test '! echo x | grep -q y; echo $?' "$(cat <<'__CASE__'
! echo x | grep -q y; echo $?
__CASE__
)"

run_test 'x=1; x=2 echo $x; echo $x' "$(cat <<'__CASE__'
x=1; x=2 echo $x; echo $x
__CASE__
)"

run_test 'readonly_test=1; unset readonly_test; echo "[${readonly_t...' "$(cat <<'__CASE__'
readonly_test=1; unset readonly_test; echo "[${readonly_test}]"
__CASE__
)"

run_test 'echo ${PATH:+has-path} ${HOME:+has-home} ${PWD:+has-pwd} ...' "$(cat <<'__CASE__'
echo ${PATH:+has-path} ${HOME:+has-home} ${PWD:+has-pwd} ${SHLVL:+has-shlvl}
__CASE__
)"

run_test 'echo "$-" | grep -c i; echo ${#-}| grep -c .' "$(cat <<'__CASE__'
echo "$-" | grep -c i; echo ${#-}| grep -c .
__CASE__
)"
