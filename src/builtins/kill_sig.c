#include "sh.h"

static const char	*sig_names(int index)
{
	static const char	*names[] = {"HUP", "INT", "QUIT", "ILL", "TRAP",
		"ABRT", "BUS", "FPE", "KILL", "USR1", "SEGV", "USR2", "PIPE",
		"ALRM", "TERM", "CHLD", "CONT", "STOP", "TSTP", "TTIN", "TTOU",
		"URG", "XCPU", "XFSZ", "VTALRM", "PROF", "WINCH", "IO", "SYS",
		NULL};

	return (names[index]);
}

static int	sig_numbers(int index)
{
	static const int	numbers[] = {SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP,
		SIGABRT, SIGBUS, SIGFPE, SIGKILL, SIGUSR1, SIGSEGV, SIGUSR2, SIGPIPE,
		SIGALRM, SIGTERM, SIGCHLD, SIGCONT, SIGSTOP, SIGTSTP, SIGTTIN,
		SIGTTOU, SIGURG, SIGXCPU, SIGXFSZ, SIGVTALRM, SIGPROF, SIGWINCH,
		SIGIO, SIGSYS};

	return (numbers[index]);
}

/*
** Signal number for a name ("TERM", "SIGTERM", any case) or a number
** in text form. -1 if it does not designate a signal.
*/
int	sig_number(const char *name)
{
	long	number;
	int	i;
	int	j;

	if (parse_long(name, &number))
	{
		if (number < 0 || number > 255)
			return (-1);
		return ((int)number);
	}
	if (ft_toupper(name[0]) == 'S' && ft_toupper(name[1]) == 'I'
		&& ft_toupper(name[2]) == 'G')
		name += 3;
	i = 0;
	while (sig_names(i))
	{
		j = 0;
		while (name[j] && ft_toupper(name[j]) == sig_names(i)[j])
			j++;
		if (!name[j] && !sig_names(i)[j])
			return (sig_numbers(i));
		i++;
	}
	return (-1);
}

/*
** Name of a signal number, NULL if it has none here.
*/
const char	*sig_name(int sig)
{
	int	i;

	i = 0;
	while (sig_names(i) && sig_numbers(i) != sig)
		i++;
	return (sig_names(i));
}
