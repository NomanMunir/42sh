#include "sh.h"

/*
** A job runs while one of its processes runs, is stopped when the others
** are all stopped or finished, and is done when all are finished.
*/
int	job_state(t_job *job)
{
	t_proc	*proc;
	int	state;

	state = JS_DONE;
	proc = job->procs;
	while (proc)
	{
		if (proc->state == JS_RUNNING)
			return (JS_RUNNING);
		if (proc->state == JS_STOPPED)
			state = JS_STOPPED;
		proc = proc->next;
	}
	return (state);
}

/*
** Exit status of a job: the one of its last process.
*/
int	job_status(t_job *job)
{
	t_proc	*proc;

	proc = job->procs;
	if (!proc)
		return (1);
	while (proc->next)
		proc = proc->next;
	return (status_code(proc->status));
}

pid_t	job_last_pid(t_job *job)
{
	t_proc	*proc;

	proc = job->procs;
	if (!proc)
		return (0);
	while (proc->next)
		proc = proc->next;
	return (proc->pid);
}

static void	proc_update(t_job *job, t_proc *proc, int wstatus)
{
	proc->status = wstatus;
	if (WIFSTOPPED(wstatus))
	{
		proc->state = JS_STOPPED;
		job->seq = ++sh()->job_seq;
		job->notified = 0;
	}
	else if (WIFCONTINUED(wstatus))
		proc->state = JS_RUNNING;
	else
	{
		proc->state = JS_DONE;
		job->notified = 0;
	}
}

/*
** Records a status change reported by waitpid for one of our children.
*/
void	jobs_update(pid_t pid, int wstatus)
{
	t_job	*job;
	t_proc	*proc;

	job = sh()->jobs;
	while (job)
	{
		proc = job->procs;
		while (proc && proc->pid != pid)
			proc = proc->next;
		if (proc)
		{
			proc_update(job, proc, wstatus);
			return ;
		}
		job = job->next;
	}
}
