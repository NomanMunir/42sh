#include "sh.h"

/*
** Lists every shell variable as name=value, sorted by name.
*/
static int	set_list_vars(void)
{
	t_buf	out;
	char	**keys;
	char	*quoted;
	int	i;

	keys = map_keys(&sh()->vars);
	buf_init(&out);
	i = 0;
	while (keys[i])
	{
		if (var_get(keys[i]))
		{
			quoted = sh_quote(var_get(keys[i]));
			buf_adds(&out, keys[i]);
			buf_addc(&out, '=');
			buf_adds(&out, quoted);
			buf_addc(&out, '\n');
			free(quoted);
		}
		i++;
	}
	free(keys);
	return (buf_write(&out, STDOUT_FILENO, "set"));
}

/*
** -o / +o with its optional operand. Returns the number of arguments
** used, or -1 on error.
*/
static int	set_dash_o(char **args)
{
	int	on;

	on = (args[0][0] == '-');
	if (!args[1])
	{
		if (set_list_options() != 0)
			return (-1);
		return (1);
	}
	if (set_option(args[1], on) != 0)
		return (-1);
	return (2);
}

static int	set_invalid(const char *arg)
{
	dprintf(STDERR_FILENO, "42sh: set: %s: invalid option\n", arg);
	dprintf(STDERR_FILENO, "set: usage: set [-o option] [+o option] "
		"[--] [argument...]\n");
	return (2);
}

/*
** set [-o option] [+o option] [--] [argument...]
** Without argument, lists the variables. Operands replace the
** positional parameters.
*/
int	bi_set(char **argv)
{
	int	used;
	int	i;

	if (!argv[1])
		return (set_list_vars());
	i = 1;
	while (argv[i] && (str_eq(argv[i], "-o") || str_eq(argv[i], "+o")))
	{
		used = set_dash_o(argv + i);
		if (used < 0)
			return (1);
		i += used;
	}
	if (argv[i] && str_eq(argv[i], "--"))
		free_strs(params_swap(strs_dup(argv + i + 1)));
	else if (argv[i] && (argv[i][0] == '-' || argv[i][0] == '+')
		&& argv[i][1])
		return (set_invalid(argv[i]));
	else if (argv[i])
		free_strs(params_swap(strs_dup(argv + i)));
	return (0);
}
