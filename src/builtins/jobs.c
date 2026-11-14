#include "sh.h"

/*
** Prints one job; a finished job is forgotten once it has been shown.
*/
static void	jobs_show(t_job *job, int mode)
{
	job_print(job, mode, STDOUT_FILENO);
	job->notified = 1;
	if (job_state(job) == JS_DONE)
		job_remove(job);
}

/*
** Prints every job first, so that the + and - marks stay consistent,
** then forgets the finished ones.
*/
static int	jobs_show_all(int mode)
{
	t_job	*job;
	t_job	*next;

	job = sh()->jobs;
	while (job)
	{
		job_print(job, mode, STDOUT_FILENO);
		job->notified = 1;
		job = job->next;
	}
	job = sh()->jobs;
	while (job)
	{
		next = job->next;
		if (job_state(job) == JS_DONE)
			job_remove(job);
		job = next;
	}
	return (0);
}

/*
** jobs [-l | -p] [job_id...]
*/
int	bi_jobs(char **argv)
{
	t_job	*job;
	int	flags;
	int	status;
	int	i;

	i = opt_parse(argv, "lp", &flags);
	if (i < 0)
		return (2);
	jobs_reap();
	if (flags & 2)
		flags = JP_PGID;
	if (!argv[i])
		return (jobs_show_all(flags));
	status = 0;
	while (argv[i])
	{
		job = job_find(argv[i]);
		if (job)
			jobs_show(job, flags);
		else
			dprintf(STDERR_FILENO, "42sh: jobs: %s: no such job\n", argv[i]);
		status |= (job == NULL);
		i++;
	}
	return (status);
}
