#!/bin/bash
# ============================================================================
# Test: Shell scripts (loops, conditions, functions) and their friends
# Every case is run by bash and by 42sh; their standard outputs must match.
# ============================================================================

section "Loops and Conditions"

run_test 'for i in 1 2 3; do echo $i; done' "$(cat <<'__CASE__'
for i in 1 2 3; do echo $i; done
__CASE__
)"

run_test 'i=0; while [ $i -lt 3 ]; do echo $i; i=$((i+1)); done' "$(cat <<'__CASE__'
i=0; while [ $i -lt 3 ]; do echo $i; i=$((i+1)); done
__CASE__
)"

run_test 'until false; do echo once; break; done' "$(cat <<'__CASE__'
until false; do echo once; break; done
__CASE__
)"

run_test 'if true; then echo yes; elif false; then echo no; else ec...' "$(cat <<'__CASE__'
if true; then echo yes; elif false; then echo no; else echo other; fi
__CASE__
)"

run_test 'if false; then echo yes; elif true; then echo elif; else ...' "$(cat <<'__CASE__'
if false; then echo yes; elif true; then echo elif; else echo other; fi
__CASE__
)"

run_test 'case abc in a*) echo match;; *) echo no;; esac' "$(cat <<'__CASE__'
case abc in a*) echo match;; *) echo no;; esac
__CASE__
)"

run_test 'case x in a|b) echo ab;; x|y) echo xy;; esac' "$(cat <<'__CASE__'
case x in a|b) echo ab;; x|y) echo xy;; esac
__CASE__
)"

run_test 'f() { echo "in f $1 $#"; return 3; }; f a b; echo $?' "$(cat <<'__CASE__'
f() { echo "in f $1 $#"; return 3; }; f a b; echo $?
__CASE__
)"

run_test 'f() { local_x=1; echo $1; }; f "a b" c' "$(cat <<'__CASE__'
f() { local_x=1; echo $1; }; f "a b" c
__CASE__
)"

run_test 'for i in 1 2 3; do for j in a b; do [ $j = b ] && continu...' "$(cat <<'__CASE__'
for i in 1 2 3; do for j in a b; do [ $j = b ] && continue 2; echo $i$j; done; done
__CASE__
)"

run_test 'for i in 1 2 3; do [ $i = 2 ] && break; echo $i; done; ec...' "$(cat <<'__CASE__'
for i in 1 2 3; do [ $i = 2 ] && break; echo $i; done; echo end
__CASE__
)"

section "Expansions in Scripts"

run_test 'x=5; echo $((x * 2 + 1)) $((x > 3)) $((x == 5 && 1)) $((x...' "$(cat <<'__CASE__'
x=5; echo $((x * 2 + 1)) $((x > 3)) $((x == 5 && 1)) $((x++)) $x $((--x)) $((10 % 3)) $((7 / 2))
__CASE__
)"

run_test 'echo ${UNSET:-default} ${UNSET:=assigned} $UNSET ${UNSET:...' "$(cat <<'__CASE__'
echo ${UNSET:-default} ${UNSET:=assigned} $UNSET ${UNSET:+alt} ${#UNSET}
__CASE__
)"

run_test 'p=/a/b/c.tar.gz; echo ${p%.*} ${p%%.*} ${p#*/} ${p##*/}' "$(cat <<'__CASE__'
p=/a/b/c.tar.gz; echo ${p%.*} ${p%%.*} ${p#*/} ${p##*/}
__CASE__
)"

run_test 'echo "$(echo "nested $(echo deep)")" `echo backtick`' "$(cat <<'__CASE__'
echo "$(echo "nested $(echo deep)")" `echo backtick`
__CASE__
)"

run_test '(cd /; pwd); pwd' "$(cat <<'__CASE__'
(cd /; pwd); pwd
__CASE__
)"

run_test '{ echo a; echo b; } | tr a-z A-Z' "$(cat <<'__CASE__'
{ echo a; echo b; } | tr a-z A-Z
__CASE__
)"

run_test 'echo a; (exit 3); echo $?' "$(cat <<'__CASE__'
echo a; (exit 3); echo $?
__CASE__
)"

section "Here-documents"

run_test 'cat <<EOF' "$(cat <<'__CASE__'
cat <<EOF
hello $HOME
  $(echo sub)
EOF
__CASE__
)"

run_test 'cat <<EOF' "$(cat <<'__CASE__'
cat <<'EOF'
no $expansion
EOF
__CASE__
)"

run_test 'cat <<-EOF' "$(cat <<'__CASE__'
cat <<-EOF
	tabbed
	EOF
__CASE__
)"

section "Redirections and Environment"

run_test 'ls /nonexistent_dir_x 2>&1 >/dev/null | wc -l' "$(cat <<'__CASE__'
ls /nonexistent_dir_x 2>&1 >/dev/null | wc -l
__CASE__
)"

run_test 'ls /nonexistent_dir_x >/dev/null 2>&1; echo $?' "$(cat <<'__CASE__'
ls /nonexistent_dir_x >/dev/null 2>&1; echo $?
__CASE__
)"

run_test 'echo hello >&2 2>/dev/null; echo ok' "$(cat <<'__CASE__'
echo hello >&2 2>/dev/null; echo ok
__CASE__
)"

run_test 'X=1 sh -c echo $X; echo "[$X]"' "$(cat <<'__CASE__'
X=1 sh -c 'echo $X'; echo "[$X]"
__CASE__
)"

run_test 'HOME=/tmp cd; pwd' "$(cat <<'__CASE__'
HOME=/tmp cd; pwd
__CASE__
)"

section "Positional Parameters and Fields"

run_test 'set -- a b c; echo $# $1 $3 "$@" "$*"; shift; echo $1 $#' "$(cat <<'__CASE__'
set -- a b c; echo $# $1 $3 "$@" "$*"; shift; echo $1 $#
__CASE__
)"

run_test 'set -- "a b" c; for x in "$@"; do echo "[$x]"; done; for ...' "$(cat <<'__CASE__'
set -- "a b" c; for x in "$@"; do echo "[$x]"; done; for x in $*; do echo "[$x]"; done
__CASE__
)"

run_test 'IFS=:; x=a:b:c; for i in $x; do echo $i; done' "$(cat <<'__CASE__'
IFS=:; x=a:b:c; for i in $x; do echo $i; done
__CASE__
)"

run_test 'echo a' "$(cat <<'__CASE__'
echo a\
b
__CASE__
)"

run_test 'echo its' "$(cat <<'__CASE__'
echo 'it'\''s'
__CASE__
)"

run_test 'echo "a$bc"d"' "$(cat <<'__CASE__'
echo "a\$b\\c\"d"
__CASE__
)"

section "Operators and Statuses"

run_test 'true && false || echo x && echo y' "$(cat <<'__CASE__'
true && false || echo x && echo y
__CASE__
)"

run_test '! true; echo $?; ! false; echo $?' "$(cat <<'__CASE__'
! true; echo $?; ! false; echo $?
__CASE__
)"

run_test 'echo $(( 1 + 2 * 3 )) $(( (1 + 2) * 3 )) $((-5)) $((1 <= ...' "$(cat <<'__CASE__'
echo $(( 1 + 2 * 3 )) $(( (1 + 2) * 3 )) $((-5)) $((1 <= 2)) $((2 != 2))
__CASE__
)"

run_test 'a=1; b=$a$a; echo ${b}2 "${a}"' "$(cat <<'__CASE__'
a=1; b=$a$a; echo ${b}2 "${a}"
__CASE__
)"

run_test 'unset X; echo ${X:-a b} "${X:-a b}"' "$(cat <<'__CASE__'
unset X; echo ${X:-a b} "${X:-a b}"
__CASE__
)"

run_test 'echo ~root | grep -c root' "$(cat <<'__CASE__'
echo ~root | grep -c root
__CASE__
)"

run_test 'cd /tmp && cd - >/dev/null && pwd' "$(cat <<'__CASE__'
cd /tmp && cd - >/dev/null && pwd
__CASE__
)"

run_test 'type cd echo type nosuch_cmd_x; echo $?' "$(cat <<'__CASE__'
type cd echo type nosuch_cmd_x; echo $?
__CASE__
)"

run_test 'export A=1 B=2; env | grep -E ^(A|B)= | sort' "$(cat <<'__CASE__'
export A=1 B=2; env | grep -E '^(A|B)=' | sort
__CASE__
)"

run_test 'A=1; export A; unset A; env | grep -c ^A=' "$(cat <<'__CASE__'
A=1; export A; unset A; env | grep -c '^A='
__CASE__
)"

run_test 'echo -n a; echo -e btcn; echo -E xty' "$(cat <<'__CASE__'
echo -n a; echo -e 'b\tc\n'; echo -E 'x\ty'
__CASE__
)"

run_test 'exit 300' "$(cat <<'__CASE__'
exit 300
__CASE__
)"

run_test 'false; exit' "$(cat <<'__CASE__'
false; exit
__CASE__
)"

section "Builtins read, eval, kill, test"

run_test 'echo a | (read x; echo "got $x")' "$(cat <<'__CASE__'
echo a | (read x; echo "got $x")
__CASE__
)"

run_test 'printf a b cn | (read x y; echo "$x|$y")' "$(cat <<'__CASE__'
printf 'a b c\n' | (read x y; echo "$x|$y")
__CASE__
)"

run_test 'printf a b cn | (read -r x y; echo "$x|$y")' "$(cat <<'__CASE__'
printf 'a\\ b c\n' | (read -r x y; echo "$x|$y")
__CASE__
)"

run_test 'printf a b cn | (read x y; echo "$x|$y")' "$(cat <<'__CASE__'
printf 'a\\ b c\n' | (read x y; echo "$x|$y")
__CASE__
)"

run_test 'printf   lead  trail  n | (read x; echo "[$x]")' "$(cat <<'__CASE__'
printf '  lead  trail  \n' | (read x; echo "[$x]")
__CASE__
)"

run_test 'read x </dev/null; echo $?' "$(cat <<'__CASE__'
read x </dev/null; echo $?
__CASE__
)"

run_test 'eval echo a; echo b; eval; echo $?' "$(cat <<'__CASE__'
eval 'echo a; echo b'; eval; echo $?
__CASE__
)"

run_test 'x=echo hi; $x; eval "$x there"' "$(cat <<'__CASE__'
x='echo hi'; $x; eval "$x there"
__CASE__
)"

run_test 'kill -l 15; kill -l TERM' "$(cat <<'__CASE__'
kill -l 15; kill -l TERM
__CASE__
)"

run_test '[ a = a ] && [ 1 -lt 2 ] && [ ! -e /nonexistent ] && [ -n...' "$(cat <<'__CASE__'
[ a = a ] && [ 1 -lt 2 ] && [ ! -e /nonexistent ] && [ -n x ] && [ -z "" ] && echo all
__CASE__
)"

run_test '[ ]; echo $?; [ x ]; echo $?; [ "" ]; echo $?' "$(cat <<'__CASE__'
[ ]; echo $?; [ x ]; echo $?; [ "" ]; echo $?
__CASE__
)"

run_test 'test 1 -eq x 2>/dev/null; echo $?' "$(cat <<'__CASE__'
test 1 -eq x 2>/dev/null; echo $?
__CASE__
)"

run_test 'echo *.nonexistent_ext "*" *' "$(cat <<'__CASE__'
echo *.nonexistent_ext "*" \*
__CASE__
)"

run_test 'cd /usr && echo b[i]n l?b' "$(cat <<'__CASE__'
cd /usr && echo b[i]n l?b
__CASE__
)"

run_test 'f() { echo $1; }; f a; unset -f f; f b 2>/dev/null; echo $?' "$(cat <<'__CASE__'
f() { echo $1; }; f a; unset -f f; f b 2>/dev/null; echo $?
__CASE__
)"

section "Compound Commands with Redirections"

run_test 'while read line; do echo "L:$line"; done <<EOF' "$(cat <<'__CASE__'
while read line; do echo "L:$line"; done <<EOF
one
two
EOF
__CASE__
)"

run_test 'echo one; echo two | while read x; do echo "got $x"; done' "$(cat <<'__CASE__'
echo one; echo two | while read x; do echo "got $x"; done
__CASE__
)"

section "Background Jobs"

run_test 'sleep 0.1 & wait; echo $?' "$(cat <<'__CASE__'
sleep 0.1 & wait; echo $?
__CASE__
)"

run_test '(exit 5) & wait $!; echo $?' "$(cat <<'__CASE__'
(exit 5) & wait $!; echo $?
__CASE__
)"

run_test 'true & echo $! | grep -c [0-9]' "$(cat <<'__CASE__'
true & echo $! | grep -c '[0-9]'
__CASE__
)"

run_test 'cat < <(echo procsub); echo $?' "$(cat <<'__CASE__'
cat < <(echo procsub); echo $?
__CASE__
)"

run_test 'echo a > /tmp/42sh_t1; cat /tmp/42sh_t1; echo b >> /tmp/4...' "$(cat <<'__CASE__'
echo a > /tmp/42sh_t1; cat /tmp/42sh_t1; echo b >> /tmp/42sh_t1; cat /tmp/42sh_t1; rm /tmp/42sh_t1
__CASE__
)"

run_test 'exec 3>/tmp/42sh_t2; echo fd3 >&3; exec 3>&-; cat /tmp/42...' "$(cat <<'__CASE__'
exec 3>/tmp/42sh_t2; echo fd3 >&3; exec 3>&-; cat /tmp/42sh_t2; rm /tmp/42sh_t2
__CASE__
)"

section "Errors and Multi-line Input"

run_test 'echo $?; nosuchcommand_zz; echo $?; /etc/passwd; echo $?;...' "$(cat <<'__CASE__'
echo $?; nosuchcommand_zz; echo $?; /etc/passwd; echo $?; /nonexistent/x; echo $?
__CASE__
)"

run_test 'x=$(false); echo $?; x=$(true); echo $?' "$(cat <<'__CASE__'
x=$(false); echo $?; x=$(true); echo $?
__CASE__
)"

run_test 'a=b c=d; echo $a $c' "$(cat <<'__CASE__'
a=b c=d; echo $a $c
__CASE__
)"

run_test 'echo "$((1+1))" $((1+1))' "$(cat <<'__CASE__'
echo "$((1+1))" '$((1+1))'
__CASE__
)"

run_test 'if [ -d /tmp ]; then' "$(cat <<'__CASE__'
if [ -d /tmp ]; then
  echo dir
fi
__CASE__
)"

run_test 'f() {' "$(cat <<'__CASE__'
f() {
  for i in 1 2; do
    echo $i
  done
}
f
__CASE__
)"

run_test 'echo a &&' "$(cat <<'__CASE__'
echo a &&
echo b
__CASE__
)"

run_test 'echo a |' "$(cat <<'__CASE__'
echo a |
cat
__CASE__
)"

run_test 'x=1; { x=2; }; echo $x; (x=3); echo $x' "$(cat <<'__CASE__'
x=1; { x=2; }; echo $x; (x=3); echo $x
__CASE__
)"

section "More Builtins"

run_test 'cd /tmp; cd ..; pwd; cd -P /tmp; pwd; pwd -P; pwd -L' "$(cat <<'__CASE__'
cd /tmp; cd ..; pwd; cd -P /tmp; pwd; pwd -P; pwd -L
__CASE__
)"

run_test 'hash -r; hash ls; hash | grep -c ls; hash nosuch_zz; echo $?' "$(cat <<'__CASE__'
hash -r; hash ls; hash | grep -c ls; hash nosuch_zz; echo $?
__CASE__
)"

run_test 'echo $(seq 1 3)' "$(cat <<'__CASE__'
echo $(seq 1 3)
__CASE__
)"

run_test 'echo "a   b" c   d e   f' "$(cat <<'__CASE__'
echo "a   b" 'c   d' e   f
__CASE__
)"

run_test 'x="a   b"; echo $x; echo "$x"' "$(cat <<'__CASE__'
x="a   b"; echo $x; echo "$x"
__CASE__
)"

run_test 'break; continue; echo $?' "$(cat <<'__CASE__'
break; continue; echo $?
__CASE__
)"

run_test 'return 2>/dev/null; echo $?' "$(cat <<'__CASE__'
return 2>/dev/null; echo $?
__CASE__
)"

run_test '. /nonexistent_file_x 2>/dev/null; echo $?' "$(cat <<'__CASE__'
. /nonexistent_file_x 2>/dev/null; echo $?
__CASE__
)"

run_test 'echo echo sourced $1 > /tmp/42sh_src; . /tmp/42sh_src a...' "$(cat <<'__CASE__'
echo 'echo sourced $1' > /tmp/42sh_src; . /tmp/42sh_src arg; rm /tmp/42sh_src
__CASE__
)"

run_test 'echo abc | { read x; echo $x; }' "$(cat <<'__CASE__'
echo abc | { read x; echo $x; }
__CASE__
)"

run_test 'jobs; fg 2>/dev/null; echo $?; bg 2>/dev/null; echo $?' "$(cat <<'__CASE__'
jobs; fg 2>/dev/null; echo $?; bg 2>/dev/null; echo $?
__CASE__
)"

run_test 'x=3; while [ $x -gt 0 ]; do x=$((x-1)); if [ $x = 1 ]; th...' "$(cat <<'__CASE__'
x=3; while [ $x -gt 0 ]; do x=$((x-1)); if [ $x = 1 ]; then continue; fi; echo $x; done
__CASE__
)"

run_test 'f() { g; echo "f:$?"; }; g() { return 7; }; f' "$(cat <<'__CASE__'
f() { g; echo "f:$?"; }; g() { return 7; }; f
__CASE__
)"

run_test 'f() { echo "$@" | wc -w; }; f a b c' "$(cat <<'__CASE__'
f() { echo "$@" | wc -w; }; f a b c
__CASE__
)"

run_test 'case "$HOME" in /*) echo abs;; esac; case foo in f?o) ech...' "$(cat <<'__CASE__'
case "$HOME" in /*) echo abs;; esac; case foo in f?o) echo q;; esac; case foo in [a-f]oo) echo r;; esac
__CASE__
)"

run_test 'echo ${X-unset} ${X+set}; X=; echo "[${X-unset}]" "[${X:-...' "$(cat <<'__CASE__'
echo ${X-unset} ${X+set}; X=; echo "[${X-unset}]" "[${X:-empty}]" ${X+set}
__CASE__
)"

run_test 's=a b; set -- $s; echo $#; set -- "$s"; echo $#' "$(cat <<'__CASE__'
s='a b'; set -- $s; echo $#; set -- "$s"; echo $#
__CASE__
)"

run_test 'echo "$(printf anbnnn)x"' "$(cat <<'__CASE__'
echo "$(printf 'a\nb\n\n\n')x"
__CASE__
)"

run_test 'cat /dev/null | cat; echo $?' "$(cat <<'__CASE__'
cat /dev/null | cat; echo $?
__CASE__
)"

run_test 'false | false | true; echo $?; true | false; echo $?' "$(cat <<'__CASE__'
false | false | true; echo $?; true | false; echo $?
__CASE__
)"

run_test 'echo hi 1>&2 | cat; echo x 2>&1 | cat' "$(cat <<'__CASE__'
echo hi 1>&2 | cat; echo x 2>&1 | cat
__CASE__
)"

run_test 'for f in /etc/host*; do echo ok; break; done' "$(cat <<'__CASE__'
for f in /etc/host*; do echo ok; break; done
__CASE__
)"

run_test 'umask >/dev/null; echo $?' "$(cat <<'__CASE__'
umask >/dev/null; echo $?
__CASE__
)"

run_test 'cd /nonexistent 2>/dev/null || echo failed; echo $?' "$(cat <<'__CASE__'
cd /nonexistent 2>/dev/null || echo failed; echo $?
__CASE__
)"

run_test 'unset HOME; cd 2>/dev/null; echo $?' "$(cat <<'__CASE__'
unset HOME; cd 2>/dev/null; echo $?
__CASE__
)"

run_test 'export X; X=5; sh -c echo $X' "$(cat <<'__CASE__'
export X; X=5; sh -c 'echo $X'
__CASE__
)"

run_test 'sleep 5 & kill %1; wait %1 2>/dev/null; echo $?' "$(cat <<'__CASE__'
sleep 5 & kill %1; wait %1 2>/dev/null; echo $?
__CASE__
)"

run_test 'sleep 5 & kill -s INT $!; wait $!; echo $?' "$(cat <<'__CASE__'
sleep 5 & kill -s INT $!; wait $!; echo $?
__CASE__
)"

run_test 'echo "a" "" b | cat -A' "$(cat <<'__CASE__'
echo "a" "" b | cat -A
__CASE__
)"

run_test 'x=; echo a $x b "$x" c | cat -A' "$(cat <<'__CASE__'
x=; echo a $x b "$x" c | cat -A
__CASE__
)"
