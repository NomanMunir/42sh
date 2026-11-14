#include "sh.h"

/*
** Splits a path into its components, dropping "." and resolving ".."
** against the component before it.
*/
static void	canon_parts(t_strv *parts, const char *path)
{
	size_t	len;

	while (*path)
	{
		len = strcspn(path, "/");
		if (len == 2 && path[0] == '.' && path[1] == '.')
		{
			if (parts->len > 0)
			{
				free(parts->v[--parts->len]);
				parts->v[parts->len] = NULL;
			}
		}
		else if (len > 0 && !(len == 1 && path[0] == '.'))
			strv_push(parts, xstrndup(path, len));
		path += len;
		path += strspn(path, "/");
	}
}

/*
** Canonical form of an absolute path, computed on the text only: no
** ".", no "..", no repeated or trailing slash.
*/
char	*path_canon(const char *path)
{
	t_strv	parts;
	t_buf	out;
	int	i;

	strv_init(&parts);
	canon_parts(&parts, path);
	buf_init(&out);
	i = 0;
	while (i < parts.len)
	{
		buf_addc(&out, '/');
		buf_adds(&out, parts.v[i++]);
	}
	if (out.len == 0)
		buf_addc(&out, '/');
	strv_free(&parts);
	return (buf_take(&out));
}

/*
** CDPATH does not apply to absolute paths nor to paths starting with
** "." or "..".
*/
static int	cdpath_applies(const char *dir)
{
	if (dir[0] == '/' || !*dir)
		return (0);
	if (dir[0] != '.')
		return (1);
	if (!dir[1] || dir[1] == '/')
		return (0);
	return (!(dir[1] == '.' && (!dir[2] || dir[2] == '/')));
}

/*
** dir below the CDPATH entry of length len (an empty entry stands for
** the current directory). NULL if that is not a directory.
*/
static char	*cdpath_try(const char *entry, size_t len, const char *dir)
{
	struct stat	info;
	char	*prefix;
	char	*path;

	if (len == 0)
		path = xstrdup(dir);
	else
	{
		prefix = xstrndup(entry, len);
		path = xjoin3(prefix, "/", dir);
		free(prefix);
	}
	if (stat(path, &info) == 0 && S_ISDIR(info.st_mode))
		return (path);
	free(path);
	return (NULL);
}

/*
** Looks dir up in the directories of CDPATH. Returns the path to change
** to (dir itself when nothing better is found); *print is set when the
** directory comes from a CDPATH entry, as its name must then be shown.
*/
char	*cd_search(const char *dir, int *print)
{
	const char	*paths;
	char	*found;
	size_t	len;

	paths = var_get("CDPATH");
	if (!paths || !cdpath_applies(dir))
		return (xstrdup(dir));
	while (1)
	{
		len = strcspn(paths, ":");
		found = cdpath_try(paths, len, dir);
		if (found)
		{
			*print = (len > 0);
			return (found);
		}
		if (!paths[len])
			return (xstrdup(dir));
		paths += len + 1;
	}
}
