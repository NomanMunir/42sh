#include "sh.h"

/*
** Optional numeric operand of break, continue, return and shift.
** Returns 0 after reporting a bad one.
*/
static int	count_operand(char **argv, long *count, long min)
{
	if (argv[1] && argv[2])
	{
		dprintf(STDERR_FILENO, "42sh: %s: too many arguments\n", argv[0]);
		return (0);
	}
	if (argv[1] && (!parse_long(argv[1], count) || *count < min))
	{
		dprintf(STDERR_FILENO, "42sh: %s: %s: invalid number\n", argv[0],
			argv[1]);
		return (0);
	}
	return (1);
}

/*
** break [n] / continue [n]: unwind out of, or to the next round of, the
** n-th enclosing loop.
*/
static int	loop_unwind(char **argv, int kind)
{
	t_shell	*s;
	long	levels;

	s = sh();
	levels = 1;
	if (!count_operand(argv, &levels, 1))
		return (1);
	if (s->loop_depth == 0)
	{
		dprintf(STDERR_FILENO, "42sh: %s: only meaningful in a loop\n",
			argv[0]);
		return (0);
	}
	if (levels > s->loop_depth)
		levels = s->loop_depth;
	s->unwind = kind;
	s->unwind_n = (int)levels;
	return (0);
}

int	bi_break(char **argv)
{
	return (loop_unwind(argv, U_BREAK));
}

int	bi_continue(char **argv)
{
	return (loop_unwind(argv, U_CONTINUE));
}

/*
** return [n]: leaves the current function or sourced file.
*/
int	bi_return(char **argv)
{
	t_shell	*s;
	long	code;

	s = sh();
	code = s->status;
	if (!count_operand(argv, &code, LONG_MIN))
		return (2);
	if (s->func_depth == 0)
	{
		dprintf(STDERR_FILENO, "42sh: return: can only return from a "
			"function or sourced script\n");
		return (2);
	}
	s->status = (int)(code & 255);
	s->unwind = U_RETURN;
	return (s->status);
}
