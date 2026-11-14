#ifndef EXEC_H
# define EXEC_H

# include "ast.h"

/*
** X_EXIT: the current process is a forked child that exits with the status
** of the node, so an external command may replace it without forking again.
*/
# define X_EXIT 1

/*
** Unwinding states: set by break, continue, return, exit or an interrupt,
** checked by every executor function on the way back up.
*/
# define U_NONE 0
# define U_BREAK 1
# define U_CONTINUE 2
# define U_RETURN 3
# define U_EXIT 4
# define U_INT 5

# define NEST_MAX 400

typedef int	(*t_execfn)(t_node *, int);

typedef struct s_fdsave
{
	int	fd;
	int	copy;
	struct s_fdsave	*next;
}	t_fdsave;

typedef struct s_tmpvar
{
	char	*name;
	void	*old;
	struct s_tmpvar	*next;
}	t_tmpvar;

int	exec_node(t_node *n, int flags);
int	exec_cmd(t_node *n, int flags);
int	exec_pipe(t_node *n, int flags);
int	exec_and(t_node *n, int flags);
int	exec_or(t_node *n, int flags);
int	exec_seq(t_node *n, int flags);
int	exec_bg(t_node *n, int flags);
int	exec_not(t_node *n, int flags);
int	exec_subshell(t_node *n, int flags);
int	exec_group(t_node *n, int flags);
int	exec_if(t_node *n, int flags);
int	exec_loop(t_node *n, int flags);
int	exec_for(t_node *n, int flags);
int	exec_case(t_node *n, int flags);
int	exec_funcdef(t_node *n, int flags);
int	exec_external(t_node *n, char **argv, int flags);
char	*cmd_resolve(const char *name);
char	*path_search(const char *name);
int	func_call(char **argv);
int	loop_control(void);
int	assign_all(char **assigns, int flags);
t_tmpvar	*assign_temp(char **assigns);
void	assign_restore(t_tmpvar *saved);
int	redir_apply(t_redir *r, t_fdsave **save);
void	redir_restore(t_fdsave **save);
void	redir_discard(t_fdsave **save);
void	fd_save(t_fdsave **save, int fd);
int	redir_open(t_tok op, const char *path);
int	redir_dup_source(t_redir *r, const char *word);
int	redir_is_both(t_redir *r);
int	heredoc_open(t_redir *r);
void	move_fd(int from, int to);

#endif
