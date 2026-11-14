#include "sh.h"

static int	is_readable_file(const char *path)
{
	struct stat	info;

	return (stat(path, &info) == 0 && S_ISREG(info.st_mode)
		&& access(path, R_OK) == 0);
}

/*
** A file named without a slash is searched in PATH (it only has to be
** readable), then in the current directory.
*/
static char	*source_find(const char *name)
{
	const char	*dirs;
	char	*dir;
	char	*path;
	size_t	len;

	dirs = var_get("PATH");
	while (dirs && !ft_strchr(name, '/'))
	{
		len = strcspn(dirs, ":");
		dir = xstrndup(dirs, len);
		if (len == 0)
			path = xstrdup(name);
		else
			path = xjoin3(dir, "/", name);
		free(dir);
		if (is_readable_file(path))
			return (path);
		free(path);
		if (!dirs[len])
			break ;
		dirs += len + 1;
	}
	return (xstrdup(name));
}

/*
** Runs the file like a function body: "return" ends it, and arguments,
** if any, are its positional parameters while it runs.
*/
static int	source_run(const char *path, char **args)
{
	t_shell	*s;
	char	**saved;
	int	status;

	s = sh();
	saved = NULL;
	if (args[0])
		saved = params_swap(strs_dup(args));
	s->func_depth++;
	status = sh_run_file(path, 0);
	s->func_depth--;
	if (s->unwind == U_RETURN)
		s->unwind = U_NONE;
	if (args[0])
		free_strs(params_swap(saved));
	return (status);
}

/*
** . file [argument...]   (also available as "source")
*/
int	bi_source(char **argv)
{
	char	*path;
	int	status;

	if (!argv[1])
	{
		dprintf(STDERR_FILENO, "42sh: %s: filename argument required\n",
			argv[0]);
		return (2);
	}
	path = source_find(argv[1]);
	status = source_run(path, argv + 2);
	free(path);
	return (status);
}
