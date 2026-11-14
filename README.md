# 42sh

A POSIX-style UNIX shell written in C for the 42 curriculum. The reference
shell is `bash`: an ordinary `sh` or `bash` user should feel at home, and the
test suite compares the output of 42sh with the output of bash.

## Build

Requirements: `gcc`, `make`, and the development files of readline and ncurses
(`libreadline-dev libncurses-dev` on Debian/Ubuntu, `readline-devel
ncurses-devel` on Fedora/RHEL). A devcontainer with everything installed is
provided in `.devcontainer/`.

| Target        | What it does                                               |
|---------------|------------------------------------------------------------|
| `make`        | builds `libft` then `42sh` (nothing is relinked needlessly) |
| `make clean`  | removes the object files                                   |
| `make fclean` | removes the object files, `libft.a` and `42sh`             |
| `make re`     | rebuilds everything                                        |
| `make debug`  | rebuilds everything with debug symbols (`-g3`)             |
| `make run`    | builds, then starts an interactive `42sh`                  |
| `make test`   | builds, then runs the test suite                           |
| `make leaks`  | builds, then starts `42sh` under valgrind                  |
| `make help`   | lists the targets                                          |

## Usage

```
./42sh                      interactive session (or commands read from a pipe)
./42sh script [args...]     runs a script
./42sh -c 'commands' [name [args...]]
```

An interactive session reads `~/.42shrc` at startup, if the file exists.

## Features

### Mandatory part

- Prompt, commands with their arguments, `PATH` lookup.
- Full line editing (readline), see [Line editing](#line-editing).
- Redirections and aggregations: `>` `>>` `<` `<<` `>&` `<&` (also `<<-`,
  `<>`, `>|`, and an explicit descriptor in front: `2>&1`, `3<file`, `2>&-`).
- Pipes `|`, separators `;`, logical operators `&&` `||`, with the grammar
  precedence of the POSIX shell.
- Internal variables (`name=value`), exported with `export`, listed with
  `set`, removed with `unset`; temporary assignment for one command
  (`HOME=/tmp cd`); `${name}` and `${?}`.
- Job control: `&`, `jobs`, `fg`, `bg`, plus `wait` and `kill` with job ids
  (`%1`, `%+`, `%-`, `%string`, `%?string`).
- Signals: `^C` interrupts the foreground job or the line being typed, `^Z`
  stops the foreground job, `^\` is ignored by the shell itself, `^D` leaves
  (with a warning first if jobs are stopped).

### Modular part

- Inhibitors `"`, `'` and `\`.
- Pattern matching: `*`, `?`, `[...]`, `[!...]`, intervals, `\`.
- Tilde expansion and the parameter formats `${p:-w}` `${p:=w}` `${p:?w}`
  `${p:+w}` `${#p}` `${p%w}` `${p%%w}` `${p#w}` `${p##w}`.
- Control groups and subshells: `( )`, `{ }`.
- Command substitution `$( )` (and backquotes).
- Arithmetic expansion `$(( ))`: `++ --`, `+ -`, `* / %`, `<= >= < >`,
  `== !=`, `&& ||`, parentheses.
- Process substitution `<( )` and `>( )`.
- History: `!!`, `!word`, `!number`, `!-number`, saved in `$HISTFILE`
  (`~/.42sh_history`) between sessions, the `fc` builtin with every POSIX
  option, incremental search with `^R`.
- Contextual completion with TAB: commands, builtins, aliases and functions
  in command position, files elsewhere, variable names after `$` and `${`.
- vi and readline editing modes, chosen with `set -o vi` / `set -o emacs`.
- Aliases: `alias`, `unalias`.
- Hash table of the commands already located, and the `hash` builtin.
- The `test` builtin (also `[`): `-b -c -d -e -f -g -L -p -r -S -s -u -w -x
  -z -n`, `= !=`, `-eq -ne -gt -ge -lt -le`, `!`, and a single operand.

### Bonus part

- Shell scripts: `if`/`elif`/`else`, `while`, `until`, `for`, `case`,
  functions, `break`, `continue`, `return`, here-documents, positional
  parameters (`$1`, `$#`, `$@`, `$*`, `shift`, `set --`).
- Completion of builtin arguments: directories for `cd`, alias names for
  `unalias`, variable names for `unset`/`export`/`read`, command names for
  `type`/`hash`/`exec`, job ids for `fg`/`bg`/`jobs`/`kill`/`wait`, option
  names for `set -o`.

## Builtins

| Builtin | Synopsis |
|---------|----------|
| `cd` | `cd [-L\|-P] [directory\|-]` (honours `CDPATH`) |
| `pwd` | `pwd [-L\|-P]` |
| `echo` | `echo [-neE] [string...]` |
| `exit` | `exit [n]` |
| `type` | `type name...` |
| `export` | `export [-p] [name[=value]...]` |
| `unset` | `unset [-fv] name...` |
| `set` | `set`, `set [-o option] [+o option] [--] [argument...]` |
| `alias`, `unalias` | `alias [-p] [name[=value]...]`, `unalias [-a] name...` |
| `hash` | `hash [-r] [utility...]` |
| `test`, `[` | `test expression`, `[ expression ]` |
| `jobs` | `jobs [-l\|-p] [job_id...]` |
| `fg`, `bg` | `fg [job_id]`, `bg [job_id...]` |
| `wait` | `wait [pid\|job_id...]` |
| `kill` | `kill [-s signal\|-signal] pid\|job_id...`, `kill -l [status...]` |
| `fc` | `fc [-r] [-e editor] [first [last]]`, `fc -l [-nr] [first [last]]`, `fc -s [old=new] [first]` |
| `history` | `history [n]`, `history -c` |
| `read` | `read [-r] [name...]` |
| `eval`, `exec` | `eval [argument...]`, `exec [command [argument...]]` |
| `.`, `source` | `. file [argument...]` |
| `break`, `continue`, `return`, `shift` | `break [n]`, `continue [n]`, `return [n]`, `shift [n]` |
| `:`, `true`, `false` | |

`env`, `setenv` and `unsetenv` are deliberately not builtins (see the subject).

## Line editing

Line editing relies on the readline library. Both modes are available:

- readline (emacs) mode, the default: `C-b C-f C-p C-n C-_ C-t A-t`, `C-a`,
  `C-e`, `C-k`, `C-r`... `C-x C-e` opens the line in an editor.
- vi mode (`set -o vi`): `# v j k l h w W e E b B ^ $ 0 | f F ; , a A i I r
  R c C S x X d D y Y p P u U`. `v` opens the line in an editor.

The editor used by `v`, `C-x C-e` and `fc` is `$FCEDIT`, else `$VISUAL`, else
`$EDITOR`, else `vi`.

The prompt comes from `PS1` (`PS2` for continuation lines), with these
escapes: `\u` user, `\h` host, `\w` current directory, `\W` its last
component, `\$` (`#` for root), `\s` shell name, `\g` git branch, `\c` green
or red depending on the last exit status, `\n`, `\e`, `\a`, `\\`, and `\[`
`\]` around sequences that do not move the cursor.

## Tests

`make test` runs every `tests/test_*.sh` file (`tests/run_tests.sh FILE` runs
one of them). Most cases feed the same input to `bash` and to `42sh` and
compare the outputs or the exit statuses; `tests/test_12_robustness.sh`
checks that hostile input (runaway recursion, deep nesting, broken syntax,
closed descriptors...) never crashes or hangs the shell.

## Source layout

```
include/        headers, one per module
libft/          the 42 C library (with its own Makefile)
src/core/       main, shell state, input sources, read-parse-run loop, signals
src/lexer/      tokens, quoting, here-document bodies
src/parser/     grammar -> syntax tree, alias substitution
src/expand/     parameters, arithmetic, command and process substitution,
                tilde, field splitting, pattern matching and globbing
src/vars/       shell variables, environment, special parameters
src/exec/       execution of the tree, pipelines, redirections
src/jobs/       job table, process groups, terminal control, waiting
src/builtins/   the builtins
src/line/       readline integration: prompt, history, completion, editor
tests/          the test suite
```
