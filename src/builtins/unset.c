#include "sh.h"

/*
** Removes a variable, or the function of that name when there is no
** such variable (or always the function with -f).
*/
static int	unset_one(const char *name, int flags)
{
	if (flags & 1)
	{
		map_del(&sh()->funcs, name);
		return (0);
	}
	if (!is_name(name))
	{
		dprintf(STDERR_FILENO, "42sh: unset: `%s': not a valid identifier\n",
			name);
		return (1);
	}
	if (map_get(&sh()->vars, name))
		var_unset(name);
	else if (!(flags & 2))
		map_del(&sh()->funcs, name);
	return (0);
}

/*
** unset [-fv] name...
*/
int	bi_unset(char **argv)
{
	int	flags;
	int	status;
	int	i;

	i = opt_parse(argv, "fv", &flags);
	if (i < 0)
		return (2);
	status = 0;
	while (argv[i])
		status |= unset_one(argv[i++], flags);
	return (status);
}
