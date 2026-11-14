#include "sh.h"

/*
** One option argument of fc: a group of the letters l, n, r, s, that
** may end with -e, whose value is the rest of the argument or the next
** one. Returns 0 after reporting an error.
*/
static int	fc_option(t_fc *fc, char **argv, int *i)
{
	static const char	*letters = "lnrs";
	const char	*arg;
	const char	*found;

	arg = argv[*i] + 1;
	while (*arg && *arg != 'e')
	{
		found = ft_strchr(letters, *arg);
		if (!found)
		{
			dprintf(STDERR_FILENO, "42sh: fc: -%c: invalid option\n", *arg);
			return (0);
		}
		fc->opts |= 1 << (found - letters);
		arg++;
	}
	if (*arg && arg[1])
		fc->editor = arg + 1;
	else if (*arg && argv[*i + 1])
		fc->editor = argv[++(*i)];
	else if (*arg)
		dprintf(STDERR_FILENO, "42sh: fc: -e: option requires an argument\n");
	return (!*arg || fc->editor != NULL);
}

/*
** An argument like -3 is an operand (a relative command number), not an
** option.
*/
static int	fc_is_option(const char *arg)
{
	return (arg && arg[0] == '-' && arg[1] && !ft_isdigit(arg[1])
		&& !str_eq(arg, "--"));
}

/*
** fc [-r] [-e editor] [first [last]]
** fc -l [-nr] [first [last]]
** fc -s [old=new] [first]
** Returns -1 after reporting a usage error.
*/
int	fc_parse(t_fc *fc, char **argv)
{
	int	i;

	ft_bzero(fc, sizeof(*fc));
	i = 1;
	while (fc_is_option(argv[i]))
	{
		if (!fc_option(fc, argv, &i))
			return (-1);
		i++;
	}
	if (argv[i] && str_eq(argv[i], "--"))
		i++;
	if ((fc->opts & FC_S) && argv[i] && ft_strchr(argv[i], '='))
		fc->subst = argv[i++];
	if (argv[i])
		fc->first = argv[i++];
	if (argv[i] && !(fc->opts & FC_S))
		fc->last = argv[i++];
	if (!argv[i])
		return (0);
	dprintf(STDERR_FILENO, "42sh: fc: too many arguments\n");
	return (-1);
}

/*
** fc: lists, edits and re-executes commands of the history.
*/
int	bi_fc(char **argv)
{
	t_fc	fc;

	if (fc_parse(&fc, argv) < 0)
		return (2);
	if (fc_range(&fc) < 0)
		return (1);
	if (fc.opts & FC_S)
		return (fc_reexec(&fc));
	if (fc.opts & FC_L)
		return (fc_list(&fc));
	return (fc_edit(&fc));
}
