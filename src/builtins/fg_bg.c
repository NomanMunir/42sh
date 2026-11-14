#include "sh.h"

/*
** Job designated by spec (the current job when spec is NULL) for fg and
** bg. NULL, after an error message, if it cannot be resumed.
*/
static t_job	*resumable_job(const char *who, const char *spec)
{
	t_job	*job;

	if (!sh()->job_control)
	{
		dprintf(STDERR_FILENO, "42sh: %s: no job control\n", who);
		return (NULL);
	}
	jobs_reap();
	job = job_find(spec);
	if (!spec)
		spec = "current";
	if (!job)
		dprintf(STDERR_FILENO, "42sh: %s: %s: no such job\n", who, spec);
	else if (job_state(job) == JS_DONE)
	{
		dprintf(STDERR_FILENO, "42sh: %s: job has terminated\n", who);
		job_remove(job);
		job = NULL;
	}
	return (job);
}

/*
** fg [job_id]: moves a job to the foreground and waits for it.
*/
int	bi_fg(char **argv)
{
	t_job	*job;
	int	flags;
	int	i;

	i = opt_parse(argv, "", &flags);
	if (i < 0)
		return (2);
	job = resumable_job("fg", argv[i]);
	if (!job)
		return (1);
	dprintf(STDOUT_FILENO, "%s\n", job->text);
	return (job_continue(job, 1));
}

static int	bg_one(const char *spec)
{
	t_job	*job;

	job = resumable_job("bg", spec);
	if (!job)
		return (1);
	if (job_state(job) == JS_RUNNING)
	{
		dprintf(STDERR_FILENO, "42sh: bg: job %d already in background\n",
			job->id);
		return (0);
	}
	dprintf(STDOUT_FILENO, "[%d] %s &\n", job->id, job->text);
	job_continue(job, 0);
	job->notified = 1;
	return (0);
}

/*
** bg [job_id...]: resumes stopped jobs in the background.
*/
int	bi_bg(char **argv)
{
	int	flags;
	int	status;
	int	i;

	i = opt_parse(argv, "", &flags);
	if (i < 0)
		return (2);
	if (!argv[i])
		return (bg_one(NULL));
	status = 0;
	while (argv[i])
		status |= bg_one(argv[i++]);
	return (status);
}
