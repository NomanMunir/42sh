#include "sh.h"

/*
** The logical current directory: $PWD when it is set, otherwise what the
** system says. NULL if neither is available.
*/
static char	*current_dir(void)
{
	char	*pwd;

	pwd = var_get("PWD");
	if (pwd && pwd[0] == '/')
		return (xstrdup(pwd));
	return (getcwd(NULL, 0));
}

/*
** Directory asked for: $HOME without operand, $OLDPWD for "-", otherwise
** the operand looked up through CDPATH.
*/
static char	*cd_target(char **args, int *print)
{
	const char	*dir;

	if (args[0] && args[1])
	{
		dprintf(STDERR_FILENO, "42sh: cd: too many arguments\n");
		return (NULL);
	}
	if (args[0] && !str_eq(args[0], "-"))
		return (cd_search(args[0], print));
	if (args[0])
		dir = var_get("OLDPWD");
	else
		dir = var_get("HOME");
	if (dir && *dir)
	{
		*print = (args[0] != NULL);
		return (xstrdup(dir));
	}
	if (args[0])
		dprintf(STDERR_FILENO, "42sh: cd: OLDPWD not set\n");
	else
		dprintf(STDERR_FILENO, "42sh: cd: HOME not set\n");
	return (NULL);
}

/*
** Logical change (-L): the new $PWD is computed from the old one, so
** that ".." undoes a symbolic link. Returns the new $PWD or NULL.
*/
static char	*cd_logical(const char *dir, const char *old)
{
	char	*joined;
	char	*canon;

	if (dir[0] == '/')
		joined = xstrdup(dir);
	else if (old)
		joined = xjoin3(old, "/", dir);
	else
		return (NULL);
	canon = path_canon(joined);
	free(joined);
	if (chdir(canon) == 0)
		return (canon);
	free(canon);
	return (NULL);
}

static int	cd_change(const char *dir, int physical)
{
	char	*old;
	char	*now;

	old = current_dir();
	now = NULL;
	if (!physical)
		now = cd_logical(dir, old);
	if (!now && chdir(dir) < 0)
	{
		dprintf(STDERR_FILENO, "42sh: cd: %s: %s\n", dir, strerror(errno));
		free(old);
		return (1);
	}
	if (!now)
		now = getcwd(NULL, 0);
	if (old)
		var_set("OLDPWD", old, V_EXPORT);
	if (now)
		var_set("PWD", now, V_EXPORT);
	free(old);
	free(now);
	return (0);
}

/*
** cd [-L | -P] [directory | -]
*/
int	bi_cd(char **argv)
{
	char	*dir;
	int	flags;
	int	first;
	int	print;
	int	status;

	first = opt_parse(argv, "LP", &flags);
	if (first < 0)
		return (2);
	print = 0;
	dir = cd_target(argv + first, &print);
	if (!dir)
		return (1);
	status = cd_change(dir, (flags & 2) && !(flags & 1));
	free(dir);
	if (status == 0 && print)
		dprintf(STDOUT_FILENO, "%s\n", var_get("PWD"));
	return (status);
}
