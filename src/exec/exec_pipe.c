#include "sh.h"

/*
** Pipelines are parsed as a left-leaning tree; this lists their commands
** from left to right (NULL-terminated array).
*/
static t_node	**pipe_flatten(t_node *n)
{
	t_node	**cmds;
	t_node	*cur;
	int	count;

	count = 1;
	cur = n;
	while (cur->type == N_PIPE)
	{
		count++;
		cur = cur->left;
	}
	cmds = xcalloc(count + 1, sizeof(t_node *));
	cur = n;
	while (cur->type == N_PIPE)
	{
		cmds[--count] = cur->right;
		cur = cur->left;
	}
	cmds[0] = cur;
	return (cmds);
}

/*
** A member of the pipeline, in its own process: connects its ends of
** the pipes and runs the command.
*/
static void	pipe_child(t_node *cmd, int input, int *fds)
{
	move_fd(input, STDIN_FILENO);
	move_fd(fds[1], STDOUT_FILENO);
	if (fds[0] >= 0)
		close(fds[0]);
	exit(exec_node(cmd, X_EXIT));
}

/*
** Forks one member of the pipeline. input is the read end left by the
** previous member (-1 for the first). Returns the read end for the next
** member, -1 after the last one, -2 on failure.
*/
static int	pipe_stage(t_job *job, t_node **cmd, int input, int bg)
{
	int	fds[2];
	pid_t	pid;

	fds[0] = -1;
	fds[1] = -1;
	if (cmd[1] && pipe(fds) < 0)
		pid = -1;
	else
		pid = job_fork(job, bg);
	if (pid == 0)
		pipe_child(cmd[0], input, fds);
	if (input >= 0)
		close(input);
	if (fds[1] >= 0)
		close(fds[1]);
	if (pid > 0)
		return (fds[0]);
	if (fds[0] >= 0)
		close(fds[0]);
	return (-2);
}

/*
** Starts every command of a pipeline in one job (one process group).
*/
t_job	*job_launch_pipeline(t_node *n, int bg)
{
	t_node	**cmds;
	t_job	*job;
	int	input;
	int	i;

	cmds = pipe_flatten(n);
	job = job_new(n->text);
	input = -1;
	i = 0;
	while (cmds[i] && input != -2)
	{
		input = pipe_stage(job, cmds + i, input, bg);
		i++;
	}
	free(cmds);
	if (input == -2)
		dprintf(STDERR_FILENO, "42sh: cannot start pipeline: %s\n",
			strerror(errno));
	return (job);
}

int	exec_pipe(t_node *n, int flags)
{
	(void)flags;
	return (job_wait_fg(job_launch_pipeline(n, 0)));
}
