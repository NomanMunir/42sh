#include "sh.h"

/*
** Waits until the job stops running and returns its status. A finished
** job is forgotten. ^C interrupts the wait (status 130), not the job.
*/
static int	wait_job(t_job *job)
{
	pid_t	pid;
	int	wstatus;
	int	status;

	while (job_state(job) == JS_RUNNING && sig_peek() != SIGINT)
	{
		pid = waitpid(-1, &wstatus, WUNTRACED);
		if (pid > 0)
			jobs_update(pid, wstatus);
		else if (errno != EINTR)
		{
			job_remove(job);
			return (127);
		}
	}
	if (job_state(job) == JS_RUNNING)
		return (128 + SIGINT);
	status = job_status(job);
	if (job_state(job) == JS_DONE)
		job_remove(job);
	return (status);
}

/*
** First job that is not stopped: still running, or finished and not
** collected yet.
*/
static t_job	*job_to_wait(void)
{
	t_job	*job;

	job = sh()->jobs;
	while (job && job_state(job) == JS_STOPPED)
		job = job->next;
	return (job);
}

/*
** Operand of wait: a job specification (%...) or a process id.
*/
static t_job	*wait_operand(const char *spec)
{
	t_job	*job;
	t_proc	*proc;
	long	pid;

	if (spec[0] == '%')
		return (job_find(spec));
	if (!parse_long(spec, &pid))
		return (NULL);
	job = sh()->jobs;
	while (job)
	{
		proc = job->procs;
		while (proc && proc->pid != pid)
			proc = proc->next;
		if (proc)
			return (job);
		job = job->next;
	}
	return (NULL);
}

/*
** Waits for every job that is still running.
*/
static int	wait_all(void)
{
	t_job	*job;

	job = job_to_wait();
	while (job && sig_peek() != SIGINT)
	{
		wait_job(job);
		job = job_to_wait();
	}
	if (sig_peek() == SIGINT)
		return (128 + SIGINT);
	return (0);
}

/*
** wait [pid | job_id...]: without operand, waits for every running job
** and returns 0; otherwise returns the status of the last operand.
*/
int	bi_wait(char **argv)
{
	t_job	*job;
	int	status;
	int	i;

	i = 1;
	if (argv[i] && str_eq(argv[i], "--"))
		i++;
	if (!argv[i])
		return (wait_all());
	status = 0;
	while (argv[i] && sig_peek() != SIGINT)
	{
		job = wait_operand(argv[i]);
		status = 127;
		if (job)
			status = wait_job(job);
		else
			dprintf(STDERR_FILENO, "42sh: wait: %s: not a child of this "
				"shell\n", argv[i]);
		i++;
	}
	return (status);
}
