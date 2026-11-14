#include "sh.h"

int	bi_true(char **argv)
{
	(void)argv;
	return (0);
}

int	bi_false(char **argv)
{
	(void)argv;
	return (1);
}

/*
** eval [argument...]: the arguments joined by spaces are run as shell
** input.
*/
int	bi_eval(char **argv)
{
	char	*text;
	int	status;

	text = strs_join(argv + 1, " ");
	status = sh_run_string(text);
	free(text);
	return (status);
}

/*
** shift [n]: drops the first n positional parameters.
*/
int	bi_shift(char **argv)
{
	t_shell	*s;
	long	count;

	s = sh();
	count = 1;
	if (argv[1] && argv[2])
	{
		dprintf(STDERR_FILENO, "42sh: shift: too many arguments\n");
		return (1);
	}
	if (argv[1] && (!parse_long(argv[1], &count) || count < 0))
	{
		dprintf(STDERR_FILENO, "42sh: shift: %s: invalid number\n", argv[1]);
		return (1);
	}
	if (count > s->npos)
		return (1);
	free_strs(params_swap(strs_dup(s->pos + count)));
	return (0);
}
