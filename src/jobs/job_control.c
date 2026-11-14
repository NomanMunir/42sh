#include "sh.h"

/*
** Takes control of the terminal for an interactive shell: waits until it
** is in the foreground, moves into its own process group, grabs the
** terminal and saves its modes. Without a usable terminal the shell
** simply runs without job control.
*/
void	jobctl_init(void)
{
	t_shell	*s;
	pid_t	foreground;

	s = sh();
	signals_interactive();
	s->tty = fcntl(STDIN_FILENO, F_DUPFD_CLOEXEC, 10);
	foreground = tcgetpgrp(s->tty);
	while (foreground >= 0 && foreground != getpgrp())
	{
		kill(-getpgrp(), SIGTTIN);
		foreground = tcgetpgrp(s->tty);
	}
	if (s->tty < 0 || foreground < 0)
		return ;
	s->pgid = getpid();
	if (setpgid(s->pgid, s->pgid) < 0)
		s->pgid = getpgrp();
	tcsetpgrp(s->tty, s->pgid);
	tcgetattr(s->tty, &s->tmodes);
	s->job_control = 1;
}

/*
** A job was started with '&': announce it and remember it for $!.
*/
void	job_background(t_job *job)
{
	job->bg = 1;
	sh()->last_bg = job_last_pid(job);
	if (sh()->interactive)
		dprintf(STDERR_FILENO, "[%d] %d\n", job->id, (int)sh()->last_bg);
}

/*
** Resumes a stopped job, in the foreground (fg) or background (bg).
** Returns the job's status for fg, 0 for bg.
*/
int	job_continue(t_job *job, int fg)
{
	t_proc	*proc;
	t_shell	*s;

	s = sh();
	proc = job->procs;
	while (proc)
	{
		if (proc->state == JS_STOPPED)
			proc->state = JS_RUNNING;
		proc = proc->next;
	}
	job->bg = !fg;
	job->seq = ++s->job_seq;
	if (fg && job->has_tmodes)
		tcsetattr(s->tty, TCSADRAIN, &job->tmodes);
	if (fg)
		tcsetpgrp(s->tty, job->pgid);
	kill(-job->pgid, SIGCONT);
	if (fg)
		return (job_wait_fg(job));
	return (0);
}
