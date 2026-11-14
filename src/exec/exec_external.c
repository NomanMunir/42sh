#include "sh.h"

/*
** execve refused a file without a "#!" line: run it as a shell script in
** this child, as POSIX asks.
*/
static void	exec_script(char *path, char **argv)
{
	free(sh()->name);
	sh()->name = xstrdup(argv[0]);
	free_strs(params_swap(strs_dup(argv + 1)));
	exit(sh_run_file(path, 0));
}

static void	exec_failed(char *path, char **argv)
{
	struct stat	info;
	int	error;

	error = errno;
	if (error == ENOEXEC)
		exec_script(path, argv);
	if (stat(path, &info) == 0 && S_ISDIR(info.st_mode))
		error = EISDIR;
	dprintf(STDERR_FILENO, "42sh: %s: %s\n", argv[0], strerror(error));
	if (error == ENOENT)
		exit(127);
	exit(126);
}

/*
** Final step in the child: redirections, exported prefix assignments,
** then the program replaces the process.
*/
static void	child_exec(t_node *n, char **argv, char *path)
{
	if (redir_apply(n->redirs, NULL) < 0)
		exit(1);
	if (assign_all(n->assigns, V_EXPORT) < 0)
		exit(1);
	execve(path, argv, vars_envp());
	exec_failed(path, argv);
}

/*
** The redirections of an unknown command are still performed, so that
** "nosuchcmd > file" creates the file like other shells do.
*/
static int	command_not_found(t_node *n, const char *name)
{
	t_fdsave	*saved;

	saved = NULL;
	if (redir_apply(n->redirs, &saved) == 0)
		dprintf(STDERR_FILENO, "42sh: %s: command not found\n", name);
	redir_restore(&saved);
	return (127);
}

/*
** Runs an external program as a foreground job and waits for it.
*/
int	exec_external(t_node *n, char **argv, int flags)
{
	char	*path;
	t_job	*job;
	pid_t	pid;

	path = cmd_resolve(argv[0]);
	if (!path)
		return (command_not_found(n, argv[0]));
	if (flags & X_EXIT)
		child_exec(n, argv, path);
	job = job_new(n->text);
	pid = job_fork(job, 0);
	if (pid == 0)
		child_exec(n, argv, path);
	free(path);
	if (pid < 0)
	{
		job_remove(job);
		return (1);
	}
	return (job_wait_fg(job));
}
