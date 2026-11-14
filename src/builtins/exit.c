#include "sh.h"

/*
** An interactive shell does not leave stopped jobs behind without a
** warning; asking a second time in a row really exits.
*/
static int	exit_blocked(void)
{
	t_shell	*s;

	s = sh();
	if (!s->interactive || s->exit_warned || !jobs_stopped())
		return (0);
	dprintf(STDERR_FILENO, "There are stopped jobs.\n");
	s->exit_warned = 2;
	return (1);
}

/*
** exit [n]
*/
int	bi_exit(char **argv)
{
	t_shell	*s;
	long	code;

	s = sh();
	if (s->interactive)
		write(STDERR_FILENO, "exit\n", 5);
	code = s->status;
	if (argv[1] && !parse_long(argv[1], &code))
	{
		dprintf(STDERR_FILENO, "42sh: exit: %s: numeric argument required\n",
			argv[1]);
		code = 2;
	}
	else if (argv[1] && argv[2])
	{
		dprintf(STDERR_FILENO, "42sh: exit: too many arguments\n");
		return (1);
	}
	if (exit_blocked())
		return (1);
	s->status = (int)(code & 255);
	s->unwind = U_EXIT;
	return (s->status);
}
