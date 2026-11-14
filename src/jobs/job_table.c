#include "sh.h"

/*
** Creates a job and registers it in the table. Its number is one more
** than the highest number in use, like in other shells.
*/
t_job	*job_new(const char *text)
{
	t_job	*job;
	t_job	**link;
	int	id;

	job = xcalloc(1, sizeof(t_job));
	if (!text)
		text = "";
	job->text = xstrdup(text);
	job->seq = ++sh()->job_seq;
	id = 0;
	link = &sh()->jobs;
	while (*link)
	{
		if ((*link)->id > id)
			id = (*link)->id;
		link = &(*link)->next;
	}
	job->id = id + 1;
	*link = job;
	return (job);
}

static void	job_free(t_job *job)
{
	t_proc	*proc;
	t_proc	*next;

	proc = job->procs;
	while (proc)
	{
		next = proc->next;
		free(proc);
		proc = next;
	}
	free(job->text);
	free(job);
}

void	job_remove(t_job *job)
{
	t_job	**link;

	link = &sh()->jobs;
	while (*link && *link != job)
		link = &(*link)->next;
	if (*link)
		*link = job->next;
	job_free(job);
}

/*
** Empties the table without touching the processes: a child shell has no
** right over the jobs of its parent.
*/
void	jobs_forget(void)
{
	t_job	*job;
	t_job	*next;

	job = sh()->jobs;
	while (job)
	{
		next = job->next;
		job_free(job);
		job = next;
	}
	sh()->jobs = NULL;
}

int	jobs_stopped(void)
{
	t_job	*job;

	job = sh()->jobs;
	while (job)
	{
		if (job_state(job) == JS_STOPPED)
			return (1);
		job = job->next;
	}
	return (0);
}
