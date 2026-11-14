#include "sh.h"

/*
** Primaries about the type and permission bits of a file.
*/
static int	test_file(char op, const char *path)
{
	struct stat	info;

	if (op == 'L' || op == 'h')
		return (lstat(path, &info) == 0 && S_ISLNK(info.st_mode));
	if (stat(path, &info) < 0)
		return (0);
	if (op == 'b')
		return (S_ISBLK(info.st_mode));
	if (op == 'c')
		return (S_ISCHR(info.st_mode));
	if (op == 'd')
		return (S_ISDIR(info.st_mode));
	if (op == 'f')
		return (S_ISREG(info.st_mode));
	if (op == 'p')
		return (S_ISFIFO(info.st_mode));
	if (op == 'S')
		return (S_ISSOCK(info.st_mode));
	if (op == 'g')
		return ((info.st_mode & S_ISGID) != 0);
	if (op == 'u')
		return ((info.st_mode & S_ISUID) != 0);
	if (op == 's')
		return (info.st_size > 0);
	return (1);
}

/*
** Unary primaries: -b -c -d -e -f -g -h -L -n -p -r -S -s -t -u -w -x -z.
** Returns 1, 0, or -1 for an unknown operator.
*/
int	test_unary(const char *op, const char *arg)
{
	long	fd;

	if (op[0] != '-' || !op[1] || op[2]
		|| !ft_strchr("bcdefghLnprSstuwxz", op[1]))
	{
		dprintf(STDERR_FILENO, "42sh: test: %s: unary operator expected\n",
			op);
		return (-1);
	}
	if (op[1] == 'z')
		return (arg[0] == '\0');
	if (op[1] == 'n')
		return (arg[0] != '\0');
	if (op[1] == 't')
		return (parse_long(arg, &fd) && fd >= 0 && fd <= INT_MAX
			&& isatty((int)fd));
	if (op[1] == 'r')
		return (access(arg, R_OK) == 0);
	if (op[1] == 'w')
		return (access(arg, W_OK) == 0);
	if (op[1] == 'x')
		return (access(arg, X_OK) == 0);
	return (test_file(op[1], arg));
}

int	test_is_binary(const char *op)
{
	static const char	*ops[] = {"=", "==", "!=", "-eq", "-ne", "-gt",
		"-ge", "-lt", "-le", NULL};
	int	i;

	i = 0;
	while (ops[i] && !str_eq(ops[i], op))
		i++;
	return (ops[i] != NULL);
}

static int	test_numbers(const char *left, const char *op, const char *right)
{
	long	a;
	long	b;

	if (!parse_long(left, &a) || !parse_long(right, &b))
	{
		if (parse_long(left, &a))
			left = right;
		dprintf(STDERR_FILENO, "42sh: test: %s: integer expression "
			"expected\n", left);
		return (-1);
	}
	if (str_eq(op, "-eq"))
		return (a == b);
	if (str_eq(op, "-ne"))
		return (a != b);
	if (str_eq(op, "-gt"))
		return (a > b);
	if (str_eq(op, "-ge"))
		return (a >= b);
	if (str_eq(op, "-lt"))
		return (a < b);
	return (a <= b);
}

/*
** Binary primaries: = != on strings, -eq -ne -gt -ge -lt -le on integers.
*/
int	test_binary(const char *left, const char *op, const char *right)
{
	if (str_eq(op, "=") || str_eq(op, "=="))
		return (str_eq(left, right));
	if (str_eq(op, "!="))
		return (!str_eq(left, right));
	return (test_numbers(left, op, right));
}
