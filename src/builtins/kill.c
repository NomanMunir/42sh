#include "sh.h"

/*
** Signals a whole job. A stopped job is also continued so that it can
** receive a terminating signal.
*/
static int	kill_job(const char *spec, int sig)
{
	t_job	*job;
	t_proc	*proc;

	job = job_find(spec);
	if (!job)
	{
		dprintf(STDERR_FILENO, "42sh: kill: %s: no such job\n", spec);
		return (1);
	}
	proc = job->procs;
	while (proc && !job->pgid)
	{
		if (proc->state != JS_DONE)
			kill(proc->pid, sig);
		proc = proc->next;
	}
	if (job->pgid)
		kill(-job->pgid, sig);
	if (job->pgid && job_state(job) == JS_STOPPED
		&& (sig == SIGTERM || sig == SIGHUP))
		kill(-job->pgid, SIGCONT);
	return (0);
}

static int	kill_one(const char *target, int sig)
{
	long	pid;

	if (target[0] == '%')
		return (kill_job(target, sig));
	if (!parse_long(target, &pid) || pid < INT_MIN || pid > INT_MAX)
	{
		dprintf(STDERR_FILENO, "42sh: kill: %s: arguments must be process "
			"or job IDs\n", target);
		return (1);
	}
	if (kill((pid_t)pid, sig) == 0)
		return (0);
	dprintf(STDERR_FILENO, "42sh: kill: (%ld) - %s\n", pid, strerror(errno));
	return (1);
}

/*
** Reads the signal option at argv[*i]: -s name, -name or -number.
** Returns the signal (SIGTERM without option), -1 if it is invalid.
*/
static int	kill_signal(char **argv, int *i)
{
	const char	*name;

	name = NULL;
	if (argv[*i] && str_eq(argv[*i], "-s"))
	{
		name = argv[*i + 1];
		*i += 1 + (name != NULL);
		if (!name)
			name = "-s";
	}
	else if (argv[*i] && argv[*i][0] == '-' && argv[*i][1]
		&& !str_eq(argv[*i], "--"))
		name = argv[(*i)++] + 1;
	if (argv[*i] && str_eq(argv[*i], "--"))
		(*i)++;
	if (!name)
		return (SIGTERM);
	if (sig_number(name) < 0)
		dprintf(STDERR_FILENO, "42sh: kill: %s: invalid signal "
			"specification\n", name);
	return (sig_number(name));
}

/*
** kill [-s signal | -signal] pid | job_id...
** kill -l [exit_status...]
*/
int	bi_kill(char **argv)
{
	int	sig;
	int	status;
	int	i;

	if (argv[1] && str_eq(argv[1], "-l"))
		return (kill_list(argv + 2));
	i = 1;
	sig = kill_signal(argv, &i);
	if (sig < 0)
		return (2);
	if (!argv[i])
	{
		dprintf(STDERR_FILENO, "kill: usage: kill [-s signal | -signal] "
			"pid | job_id...\n");
		return (2);
	}
	status = 0;
	while (argv[i])
		status |= kill_one(argv[i++], sig);
	return (status);
}
