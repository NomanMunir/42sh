#include "sh.h"

/*
** Collects every pending status change without blocking.
*/
void	jobs_reap(void)
{
	pid_t	pid;
	int	wstatus;

	pid = 1;
	while (pid > 0)
	{
		pid = waitpid(-1, &wstatus, WNOHANG | WUNTRACED | WCONTINUED);
		if (pid > 0)
			jobs_update(pid, wstatus);
	}
}

/*
** Blocks until the job is no longer running (finished or stopped).
*/
void	job_wait(t_job *job)
{
	pid_t	pid;
	int	wstatus;
	int	options;

	options = 0;
	if (sh()->job_control)
		options = WUNTRACED;
	while (job_state(job) == JS_RUNNING)
	{
		pid = waitpid(-1, &wstatus, options);
		if (pid > 0)
			jobs_update(pid, wstatus);
		else if (errno != EINTR)
			break ;
	}
}

/*
** A foreground job that was killed by a signal: say which one, the way
** users expect ("Segmentation fault (core dumped)"). ^C just gets its
** newline, and in an interactive shell cancels the rest of the line.
*/
static void	report_signal(t_job *job)
{
	t_proc	*proc;
	int	sig;

	proc = job->procs;
	while (proc->next)
		proc = proc->next;
	if (!WIFSIGNALED(proc->status))
		return ;
	sig = WTERMSIG(proc->status);
	if (sig == SIGINT)
		write(STDERR_FILENO, "\n", 1);
	else if (sig != SIGPIPE && WCOREDUMP(proc->status))
		dprintf(STDERR_FILENO, "%s (core dumped)\n", strsignal(sig));
	else if (sig != SIGPIPE)
		dprintf(STDERR_FILENO, "%s\n", strsignal(sig));
	if (sig == SIGINT && sh()->interactive)
		sh()->unwind = U_INT;
}

/*
** The foreground job stopped or ended. A stopped job stays in the table
** as a background job; a finished one is forgotten.
*/
static int	job_finish(t_job *job)
{
	int	status;

	status = job_status(job);
	if (job_state(job) == JS_STOPPED)
	{
		job->bg = 1;
		job->notified = 1;
		write(STDERR_FILENO, "\n", 1);
		job_print(job, JP_NORMAL, STDERR_FILENO);
		sh()->status = status;
		sh()->unwind = U_INT;
		return (status);
	}
	if (job->procs)
		report_signal(job);
	job_remove(job);
	if (sh()->unwind == U_INT)
		sh()->status = status;
	return (status);
}

/*
** Gives the terminal to the job, waits for it, then takes the terminal
** back and restores the shell's terminal modes (the job may have left
** the terminal in any state).
*/
int	job_wait_fg(t_job *job)
{
	t_shell	*s;

	s = sh();
	if (s->job_control && job->pgid)
		tcsetpgrp(s->tty, job->pgid);
	job_wait(job);
	if (s->job_control)
	{
		tcsetpgrp(s->tty, s->pgid);
		if (job_state(job) == JS_STOPPED)
		{
			tcgetattr(s->tty, &job->tmodes);
			job->has_tmodes = 1;
		}
		tcsetattr(s->tty, TCSADRAIN, &s->tmodes);
	}
	return (job_finish(job));
}
