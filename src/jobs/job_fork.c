#include "sh.h"

/*
** State of a freshly forked shell process: no prompt, no job control and
** no jobs of its own yet. With job set, the terminal stop signals get
** their default action back too (substitution children keep ignoring
** them so that ^Z cannot freeze the shell that waits for them).
*/
void	sh_enter_child(int job)
{
	t_shell	*s;

	s = sh();
	if (s->interactive)
		signals_default(job);
	s->interactive = 0;
	s->job_control = 0;
	s->is_child = 1;
	s->nprocsub = 0;
	jobs_forget();
}

/*
** An asynchronous command of a shell without job control ignores the
** keyboard signals and does not read the shell's input.
*/
static void	detach_async(void)
{
	int	fd;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	fd = open("/dev/null", O_RDONLY);
	if (fd >= 0)
		move_fd(fd, STDIN_FILENO);
}

static void	child_setup(t_job *job, int bg)
{
	t_shell	*s;
	pid_t	pgid;

	s = sh();
	if (s->job_control)
	{
		pgid = job->pgid;
		if (pgid == 0)
			pgid = getpid();
		setpgid(0, pgid);
		if (!bg)
			tcsetpgrp(s->tty, pgid);
	}
	else if (bg)
		detach_async();
	sh_enter_child(1);
}

static void	parent_setup(t_job *job, pid_t pid)
{
	t_proc	*proc;
	t_proc	**link;

	if (sh()->job_control)
	{
		if (job->pgid == 0)
			job->pgid = pid;
		setpgid(pid, job->pgid);
	}
	proc = xcalloc(1, sizeof(t_proc));
	proc->pid = pid;
	proc->state = JS_RUNNING;
	link = &job->procs;
	while (*link)
		link = &(*link)->next;
	*link = proc;
}

/*
** Forks a process belonging to job. With job control every job gets its
** own process group, and a foreground job gets the terminal. Both the
** parent and the child set the group, to avoid any race between them.
*/
pid_t	job_fork(t_job *job, int bg)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		dprintf(STDERR_FILENO, "42sh: fork: %s\n", strerror(errno));
	else if (pid == 0)
		child_setup(job, bg);
	else
		parent_setup(job, pid);
	return (pid);
}
