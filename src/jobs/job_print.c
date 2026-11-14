#include "sh.h"

static const char	*stop_reason(int sig)
{
	if (sig == SIGTTIN)
		return ("Stopped (tty input)");
	if (sig == SIGTTOU)
		return ("Stopped (tty output)");
	if (sig == SIGSTOP)
		return ("Stopped (signal)");
	return ("Stopped");
}

/*
** Human readable state: Running, Stopped, Done, Exit N or the name of the
** signal that killed the job.
*/
static void	job_state_text(t_job *job, char *text, size_t size)
{
	t_proc	*proc;
	int	state;

	state = job_state(job);
	proc = job->procs;
	while (proc && proc->next && !(state == JS_STOPPED
			&& proc->state == JS_STOPPED))
		proc = proc->next;
	if (state == JS_RUNNING || !proc)
		snprintf(text, size, "Running");
	else if (state == JS_STOPPED)
		snprintf(text, size, "%s", stop_reason(WSTOPSIG(proc->status)));
	else if (WIFSIGNALED(proc->status))
		snprintf(text, size, "%s", strsignal(WTERMSIG(proc->status)));
	else if (WEXITSTATUS(proc->status) == 0)
		snprintf(text, size, "Done");
	else
		snprintf(text, size, "Exit %d", WEXITSTATUS(proc->status));
}

/*
** One line of the jobs listing:  [1]+  Running                 sleep 9 &
*/
void	job_print(t_job *job, int mode, int fd)
{
	char	state[64];
	char	mark;
	const char	*suffix;

	if (mode == JP_PGID)
	{
		dprintf(fd, "%d\n", (int)job->procs->pid);
		return ;
	}
	mark = ' ';
	if (job == job_current(NULL))
		mark = '+';
	else if (job == job_current(job_current(NULL)))
		mark = '-';
	suffix = "";
	if (job_state(job) == JS_RUNNING)
		suffix = " &";
	job_state_text(job, state, sizeof(state));
	if (mode == JP_LONG)
		dprintf(fd, "[%d]%c %d %-23s %s%s\n", job->id, mark,
			(int)job->procs->pid, state, job->text, suffix);
	else
		dprintf(fd, "[%d]%c  %-24s%s%s\n", job->id, mark, state, job->text,
			suffix);
}

/*
** Reports the jobs whose state changed since the last prompt and drops
** the finished ones. A non-interactive shell keeps finished jobs around
** so that "wait" can still return their status.
*/
void	jobs_notify(void)
{
	t_job	*job;
	t_job	*next;

	jobs_reap();
	job = sh()->jobs;
	while (job && sh()->interactive)
	{
		next = job->next;
		if (job_state(job) == JS_DONE)
		{
			if (job->bg)
				job_print(job, JP_NORMAL, STDERR_FILENO);
			job_remove(job);
		}
		else if (job_state(job) == JS_STOPPED && !job->notified)
		{
			job_print(job, JP_NORMAL, STDERR_FILENO);
			job->notified = 1;
		}
		job = next;
	}
}
