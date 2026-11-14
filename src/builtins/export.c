#include "sh.h"

static void	export_print(t_buf *out, const char *name)
{
	char	*quoted;

	buf_adds(out, "export ");
	buf_adds(out, name);
	if (var_get(name))
	{
		quoted = sh_quote(var_get(name));
		buf_addc(out, '=');
		buf_adds(out, quoted);
		free(quoted);
	}
	buf_addc(out, '\n');
}

/*
** Lists the exported variables in a form that can be read back.
*/
static int	export_list(void)
{
	t_buf	out;
	char	**keys;
	int	i;

	keys = map_keys(&sh()->vars);
	buf_init(&out);
	i = 0;
	while (keys[i])
	{
		if (var_exported(keys[i]))
			export_print(&out, keys[i]);
		i++;
	}
	free(keys);
	return (buf_write(&out, STDOUT_FILENO, "export"));
}

/*
** name or name=value: marks the variable for export, with a new value
** in the second form.
*/
static int	export_one(const char *arg)
{
	char	*name;
	size_t	len;

	len = name_len(arg);
	if (len == 0 || (arg[len] && arg[len] != '='))
	{
		dprintf(STDERR_FILENO, "42sh: export: `%s': not a valid identifier\n",
			arg);
		return (1);
	}
	name = xstrndup(arg, len);
	if (arg[len] == '=')
		var_set(name, arg + len + 1, V_EXPORT);
	else
		var_set(name, NULL, V_EXPORT);
	free(name);
	return (0);
}

/*
** export [-p] [name[=value]...]
*/
int	bi_export(char **argv)
{
	int	flags;
	int	status;
	int	i;

	i = opt_parse(argv, "p", &flags);
	if (i < 0)
		return (2);
	if (!argv[i])
		return (export_list());
	status = 0;
	while (argv[i])
		status |= export_one(argv[i++]);
	return (status);
}
