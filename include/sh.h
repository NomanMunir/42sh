#ifndef SH_H
# define SH_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <limits.h>
# include <dirent.h>
# include <termios.h>
# include <pwd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft.h"
# include "utils.h"
# include "lexer.h"
# include "ast.h"
# include "expand.h"
# include "jobs.h"
# include "exec.h"
# include "builtins.h"
# include "line.h"

# define V_EXPORT 1

# define IN_TTY 0
# define IN_FD 1
# define IN_STR 2

# define PROCSUB_MAX 32

typedef struct s_var
{
	char	*value;
	int	flags;
}	t_var;

typedef struct s_hashent
{
	char	*path;
	int	hits;
}	t_hashent;

/*
** One source of commands: the terminal (readline), a file descriptor read
** byte by byte, or a string held in memory (-c, scripts, eval, $(...)).
*/
typedef struct s_input
{
	int	kind;
	int	fd;
	const char	*str;
}	t_input;

typedef struct s_shell
{
	t_map	vars;
	t_map	aliases;
	t_map	funcs;
	t_map	hash;
	t_job	*jobs;
	char	*name;
	char	**pos;
	int	npos;
	int	status;
	int	subst_status;
	pid_t	pid;
	pid_t	pgid;
	pid_t	last_bg;
	int	interactive;
	int	job_control;
	int	is_child;
	int	tty;
	struct termios	tmodes;
	int	unwind;
	int	unwind_n;
	int	loop_depth;
	int	func_depth;
	int	nest;
	int	arith_depth;
	int	exit_warned;
	int	keep_fds;
	int	vi_mode;
	long	job_seq;
	int	procsub[PROCSUB_MAX];
	int	nprocsub;
	t_strv	comp;
	int	comp_i;
}	t_shell;

t_shell	*sh(void);
void	sh_init(char **argv, char **envp);
void	sh_destroy(void);
void	sh_loop(t_input *in);
void	run_input(t_input *in);
void	run_text(t_input *in, char *line);
int	sh_run_string(const char *str);
int	sh_run_file(const char *path, int toplevel);
char	*file_read_all(int fd);
char	*input_line(t_input *in, int cont);
void	signals_interactive(void);
void	signals_default(int job);
int	sig_peek(void);
int	sig_take(void);
char	*var_get(const char *name);
void	var_set(const char *name, const char *value, int flags);
void	var_unset(const char *name);
void	var_free(void *var);
int	var_exported(const char *name);
void	var_restore(const char *name, t_var *old);
t_var	*var_copy(const char *name);
void	vars_import(char **envp);
void	vars_defaults(void);
char	**vars_envp(void);
char	*param_value(const char *name);
char	**params_swap(char **params);
char	*params_join(void);

#endif
