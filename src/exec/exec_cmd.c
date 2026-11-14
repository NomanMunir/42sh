#include "sh.h"

/*
** A command made only of assignments and redirections: the assignments
** modify the shell itself. $? is the status of the last substitution.
*/
static int	exec_assignments(t_node *n)
{
	t_fdsave	*saved;
	int	status;

	saved = NULL;
	status = 0;
	if (redir_apply(n->redirs, &saved) < 0)
		status = 1;
	redir_restore(&saved);
	if (status == 0 && assign_all(n->assigns, 0) < 0)
		status = 1;
	if (status == 0)
		status = sh()->subst_status;
	return (status);
}

/*
** Builtins and functions run inside the shell: redirections and prefix
** assignments are applied around the call and undone afterwards.
*/
static int	exec_inshell(t_node *n, char **argv, t_builtin builtin)
{
	t_fdsave	*saved;
	t_tmpvar	*vars;
	int	status;

	saved = NULL;
	if (redir_apply(n->redirs, &saved) < 0)
	{
		redir_restore(&saved);
		return (1);
	}
	vars = assign_temp(n->assigns);
	if (builtin)
		status = builtin(argv);
	else
		status = func_call(argv);
	assign_restore(vars);
	if (sh()->keep_fds)
		redir_discard(&saved);
	else
		redir_restore(&saved);
	sh()->keep_fds = 0;
	return (status);
}

static int	exec_argv(t_node *n, char **argv, int flags)
{
	t_builtin	builtin;

	if (map_get(&sh()->funcs, argv[0]))
		return (exec_inshell(n, argv, NULL));
	builtin = builtin_find(argv[0]);
	if (builtin)
		return (exec_inshell(n, argv, builtin));
	return (exec_external(n, argv, flags));
}

/*
** Simple command: expand the words, then run a function, a builtin or an
** external program, in that order of preference.
*/
int	exec_cmd(t_node *n, int flags)
{
	char	**argv;
	int	status;

	sh()->subst_status = 0;
	argv = expand_words(n->words);
	if (!argv)
		status = 1;
	else if (sh()->interactive && sig_peek() == SIGINT)
		status = 130;
	else if (!argv[0])
		status = exec_assignments(n);
	else
		status = exec_argv(n, argv, flags);
	free_strs(argv);
	procsub_close();
	return (status);
}
