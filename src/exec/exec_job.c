#include "sh.h"

/*
** Runs a whole tree in one child process that becomes a job of its own.
*/
t_job	*job_launch_single(t_node *n, int bg)
{
	t_job	*job;
	pid_t	pid;

	job = job_new(n->text);
	pid = job_fork(job, bg);
	if (pid == 0)
		exit(exec_node(n, X_EXIT));
	return (job);
}

/*
** command &: the job is started and the shell goes on without waiting.
*/
int	exec_bg(t_node *n, int flags)
{
	t_job	*job;

	(void)flags;
	if (n->left->type == N_PIPE)
		job = job_launch_pipeline(n->left, 1);
	else
		job = job_launch_single(n->left, 1);
	if (!job->procs)
	{
		job_remove(job);
		return (1);
	}
	job_background(job);
	return (0);
}

/*
** ( list ): the list runs in a child shell. When this process is already
** a disposable child, it can simply run the list itself.
*/
int	exec_subshell(t_node *n, int flags)
{
	t_job	*job;

	if (flags & X_EXIT)
	{
		if (redir_apply(n->redirs, NULL) < 0)
			return (1);
		return (exec_node(n->left, X_EXIT));
	}
	job = job_launch_single(n, 0);
	return (job_wait_fg(job));
}

static void	func_free(void *body)
{
	node_free(body);
}

int	exec_funcdef(t_node *n, int flags)
{
	(void)flags;
	if (!sh()->funcs.del)
		sh()->funcs.del = func_free;
	map_set(&sh()->funcs, n->words[0], node_dup(n->left));
	return (0);
}
