#include "sh.h"

/*
** A non-interactive shell does not survive a failed exec.
*/
static int	exec_failure(int status)
{
	if (sh()->interactive)
	{
		signals_interactive();
		return (status);
	}
	sh()->status = status;
	sh()->unwind = U_EXIT;
	return (status);
}

/*
** Replaces the shell by the command. The redirections and the prefix
** assignments of the exec command are already in place.
*/
static int	exec_replace(char **argv)
{
	char	*path;
	char	**env;
	int	error;

	path = cmd_resolve(argv[0]);
	if (!path)
	{
		dprintf(STDERR_FILENO, "42sh: exec: %s: not found\n", argv[0]);
		return (exec_failure(127));
	}
	if (sh()->interactive)
	{
		hist_save();
		signals_default(1);
	}
	env = vars_envp();
	execve(path, argv, env);
	error = errno;
	dprintf(STDERR_FILENO, "42sh: exec: %s: %s\n", argv[0], strerror(error));
	free_strs(env);
	free(path);
	if (error == ENOENT)
		return (exec_failure(127));
	return (exec_failure(126));
}

/*
** exec [command [argument...]]: without command, the redirections of
** the exec command itself become permanent for the shell.
*/
int	bi_exec(char **argv)
{
	int	i;

	i = 1;
	if (argv[i] && str_eq(argv[i], "--"))
		i++;
	if (argv[i])
		return (exec_replace(argv + i));
	sh()->keep_fds = 1;
	return (0);
}
