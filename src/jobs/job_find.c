#include "sh.h"

/*
** Stopped jobs come before running ones, then the most recently used.
*/
static long	job_rank(t_job *job)
{
	if (job_state(job) == JS_STOPPED)
		return (job->seq + (1L << 40));
	return (job->seq);
}

/*
** The current job (%+), or with exclude set the best job after it (%-).
*/
t_job	*job_current(t_job *exclude)
{
	t_job	*job;
	t_job	*best;

	best = NULL;
	job = sh()->jobs;
	while (job)
	{
		if (job != exclude && (!best || job_rank(job) > job_rank(best)))
			best = job;
		job = job->next;
	}
	return (best);
}

static t_job	*job_by_id(long id)
{
	t_job	*job;

	job = sh()->jobs;
	while (job && job->id != id)
		job = job->next;
	return (job);
}

/*
** %string: job whose command starts with string; %?string: contains it.
*/
static t_job	*job_by_text(const char *text)
{
	t_job	*job;
	size_t	len;

	job = sh()->jobs;
	len = ft_strlen(text);
	while (job)
	{
		if (text[0] == '?' && ft_strnstr(job->text, text + 1,
				ft_strlen(job->text)))
			return (job);
		if (text[0] != '?' && ft_strncmp(job->text, text, len) == 0)
			return (job);
		job = job->next;
	}
	return (NULL);
}

/*
** Resolves a job specification: %n, %%, %+, %-, %string, %?string, or a
** bare number. NULL or "%" means the current job.
*/
t_job	*job_find(const char *spec)
{
	long	id;

	if (spec && spec[0] == '%')
		spec++;
	if (!spec || !*spec || str_eq(spec, "%") || str_eq(spec, "+"))
		return (job_current(NULL));
	if (str_eq(spec, "-"))
		return (job_current(job_current(NULL)));
	if (parse_long(spec, &id))
		return (job_by_id(id));
	return (job_by_text(spec));
}
