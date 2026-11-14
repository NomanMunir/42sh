#include "sh.h"

static const char	*type_kind(const char *name)
{
	if (is_reserved(name))
		return (" is a shell keyword\n");
	if (map_get(&sh()->funcs, name))
		return (" is a function\n");
	if (builtin_find(name))
		return (" is a shell builtin\n");
	return (NULL);
}

/*
** Describes a name that is not an external command: alias, reserved
** word, function or builtin. Returns 0 if it is none of those.
*/
static int	type_internal(t_buf *out, const char *name)
{
	const char	*alias;
	const char	*kind;

	alias = map_get(&sh()->aliases, name);
	kind = type_kind(name);
	if (!alias && !kind)
		return (0);
	buf_adds(out, name);
	if (alias)
	{
		buf_adds(out, " is aliased to `");
		buf_adds(out, alias);
		buf_adds(out, "'\n");
	}
	else
		buf_adds(out, kind);
	return (1);
}

/*
** Path of the external command called name, NULL if there is none. A
** name with a slash must be an executable file itself.
*/
static char	*type_path(const char *name, int *hashed)
{
	struct stat	info;
	t_hashent	*entry;

	*hashed = 0;
	if (ft_strchr(name, '/'))
	{
		if (stat(name, &info) == 0 && S_ISREG(info.st_mode)
			&& access(name, X_OK) == 0)
			return (xstrdup(name));
		return (NULL);
	}
	entry = map_get(&sh()->hash, name);
	if (entry && access(entry->path, X_OK) == 0)
	{
		*hashed = 1;
		return (xstrdup(entry->path));
	}
	return (path_search(name));
}

static int	type_external(t_buf *out, const char *name)
{
	char	*path;
	int	hashed;

	path = type_path(name, &hashed);
	if (!path)
		return (0);
	buf_adds(out, name);
	if (hashed)
		buf_adds(out, " is hashed (");
	else
		buf_adds(out, " is ");
	buf_adds(out, path);
	if (hashed)
		buf_addc(out, ')');
	buf_addc(out, '\n');
	free(path);
	return (1);
}

/*
** type name...: tells how each name would be interpreted as a command.
*/
int	bi_type(char **argv)
{
	t_buf	out;
	int	status;
	int	i;

	buf_init(&out);
	status = 0;
	i = 1;
	if (argv[i] && str_eq(argv[i], "--"))
		i++;
	while (argv[i])
	{
		if (!type_internal(&out, argv[i]) && !type_external(&out, argv[i]))
		{
			dprintf(STDERR_FILENO, "42sh: type: %s: not found\n", argv[i]);
			status = 1;
		}
		i++;
	}
	if (buf_write(&out, STDOUT_FILENO, "type"))
		return (1);
	return (status);
}
