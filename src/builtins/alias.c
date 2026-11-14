#include "sh.h"

static void	alias_print(t_buf *out, const char *name)
{
	char	*quoted;

	quoted = sh_quote(map_get(&sh()->aliases, name));
	buf_adds(out, "alias ");
	buf_adds(out, name);
	buf_addc(out, '=');
	if (quoted[0] != '\'')
		buf_addc(out, '\'');
	buf_adds(out, quoted);
	if (quoted[0] != '\'')
		buf_addc(out, '\'');
	buf_addc(out, '\n');
	free(quoted);
}

static void	alias_list(t_buf *out)
{
	char	**keys;
	int	i;

	keys = map_keys(&sh()->aliases);
	i = 0;
	while (keys[i])
		alias_print(out, keys[i++]);
	free(keys);
}

/*
** name=value defines an alias, name alone shows it.
*/
static int	alias_one(t_buf *out, const char *arg)
{
	const char	*equal;
	char	*name;

	equal = ft_strchr(arg + (arg[0] != '\0'), '=');
	if (equal)
	{
		name = xstrndup(arg, equal - arg);
		map_set(&sh()->aliases, name, xstrdup(equal + 1));
		free(name);
		return (0);
	}
	if (map_get(&sh()->aliases, arg))
	{
		alias_print(out, arg);
		return (0);
	}
	dprintf(STDERR_FILENO, "42sh: alias: %s: not found\n", arg);
	return (1);
}

/*
** alias [-p] [name[=value]...]
*/
int	bi_alias(char **argv)
{
	t_buf	out;
	int	flags;
	int	status;
	int	i;

	i = opt_parse(argv, "p", &flags);
	if (i < 0)
		return (2);
	buf_init(&out);
	status = 0;
	if (!argv[i] || flags)
		alias_list(&out);
	while (argv[i])
		status |= alias_one(&out, argv[i++]);
	if (buf_write(&out, STDOUT_FILENO, "alias"))
		return (1);
	return (status);
}

/*
** unalias name... | unalias -a
*/
int	bi_unalias(char **argv)
{
	int	flags;
	int	status;
	int	i;

	i = opt_parse(argv, "a", &flags);
	if (i < 0 || (!argv[i] && !flags))
	{
		if (i >= 0)
			dprintf(STDERR_FILENO, "unalias: usage: unalias [-a] name...\n");
		return (2);
	}
	if (flags)
		map_clear(&sh()->aliases);
	status = 0;
	while (argv[i])
	{
		if (!map_del(&sh()->aliases, argv[i]))
		{
			dprintf(STDERR_FILENO, "42sh: unalias: %s: not found\n", argv[i]);
			status = 1;
		}
		i++;
	}
	return (status);
}
