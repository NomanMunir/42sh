#include "sh.h"

static int	test_expr(char **args, int count);

static int	test_syntax(const char *what, const char *arg)
{
	dprintf(STDERR_FILENO, "42sh: test: %s: %s\n", arg, what);
	return (-1);
}

/*
** Three arguments: a binary primary, a negated two-argument test or a
** parenthesised single argument.
*/
static int	test_three(char **args)
{
	int	result;

	if (test_is_binary(args[1]))
		return (test_binary(args[0], args[1], args[2]));
	if (str_eq(args[0], "!"))
	{
		result = test_expr(args + 1, 2);
		if (result < 0)
			return (result);
		return (!result);
	}
	if (str_eq(args[0], "(") && str_eq(args[2], ")"))
		return (args[1][0] != '\0');
	return (test_syntax("binary operator expected", args[1]));
}

/*
** Evaluates an expression following the POSIX rules, which depend on
** the number of arguments. Returns 1 (true), 0 (false) or -1 (error).
*/
static int	test_expr(char **args, int count)
{
	int	result;

	if (count == 0)
		return (0);
	if (count == 1)
		return (args[0][0] != '\0');
	if (count == 3)
		return (test_three(args));
	if (str_eq(args[0], "!"))
	{
		result = test_expr(args + 1, count - 1);
		if (result < 0)
			return (result);
		return (!result);
	}
	if (count == 2)
		return (test_unary(args[0], args[1]));
	if (count == 4 && str_eq(args[0], "(") && str_eq(args[3], ")"))
		return (test_expr(args + 1, 2));
	dprintf(STDERR_FILENO, "42sh: test: too many arguments\n");
	return (-1);
}

/*
** test expression    /    [ expression ]
** Exit status: 0 if the expression is true, 1 if it is false, 2 on error.
*/
int	bi_test(char **argv)
{
	int	count;
	int	result;

	count = strs_len(argv) - 1;
	if (str_eq(argv[0], "["))
	{
		if (count < 1 || !str_eq(argv[count], "]"))
		{
			dprintf(STDERR_FILENO, "42sh: [: missing `]'\n");
			return (2);
		}
		count--;
	}
	result = test_expr(argv + 1, count);
	if (result < 0)
		return (2);
	return (!result);
}
