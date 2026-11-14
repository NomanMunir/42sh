#include "sh.h"

static int	is_executable(const char *path)
{
	struct stat	info;

	return (stat(path, &info) == 0 && S_ISREG(info.st_mode)
		&& access(path, X_OK) == 0);
}

/*
** Looks for an executable called name in the directories of dirs
** (colon separated; an empty entry means the current directory).
*/
static char	*search_dirs(const char *dirs, const char *name)
{
	char	*dir;
	char	*path;
	size_t	len;

	while (1)
	{
		len = strcspn(dirs, ":");
		dir = xstrndup(dirs, len);
		if (len == 0)
			path = xjoin("./", name);
		else
			path = xjoin3(dir, "/", name);
		free(dir);
		if (is_executable(path))
			return (path);
		free(path);
		if (!dirs[len])
			return (NULL);
		dirs += len + 1;
	}
}

char	*path_search(const char *name)
{
	const char	*dirs;

	dirs = var_get("PATH");
	if (!dirs || !*name)
		return (NULL);
	return (search_dirs(dirs, name));
}

static void	hash_remember(const char *name, const char *path)
{
	t_hashent	*entry;

	entry = xcalloc(1, sizeof(t_hashent));
	entry->path = xstrdup(path);
	entry->hits = 1;
	map_set(&sh()->hash, name, entry);
}

/*
** Full path of the program to run for name, NULL if there is none. A name
** containing a slash is used as is; others go through the hash table of
** already located commands, then through PATH.
*/
char	*cmd_resolve(const char *name)
{
	t_hashent	*entry;
	char	*path;

	if (ft_strchr(name, '/'))
		return (xstrdup(name));
	entry = map_get(&sh()->hash, name);
	if (entry && is_executable(entry->path))
	{
		entry->hits++;
		return (xstrdup(entry->path));
	}
	path = path_search(name);
	if (path)
		hash_remember(name, path);
	else if (entry)
		map_del(&sh()->hash, name);
	return (path);
}
