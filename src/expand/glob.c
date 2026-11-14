#include "sh.h"

static void	glob_walk(t_strv *out, const char *prefix, const char *pattern);

char	*pat_unescape(const char *pat)
{
	t_buf	buf;

	buf_init(&buf);
	while (*pat)
	{
		if (*pat == '\\' && pat[1])
			pat++;
		buf_addc(&buf, *pat++);
	}
	return (buf_take(&buf));
}

/*
** path matched the current component (it is consumed here). Either it is
** a result, or the walk goes on inside it with the rest of the pattern.
*/
static void	glob_descend(t_strv *out, char *path, const char *rest)
{
	struct stat	info;
	char	*dir;

	if (!*rest)
	{
		if (lstat(path, &info) == 0)
			strv_push(out, path);
		else
			free(path);
		return ;
	}
	dir = NULL;
	if (stat(path, &info) == 0 && S_ISDIR(info.st_mode))
		dir = xjoin(path, "/");
	free(path);
	rest += strspn(rest, "/");
	if (dir && !*rest)
		strv_push(out, dir);
	else if (dir)
	{
		glob_walk(out, dir, rest);
		free(dir);
	}
}

/*
** Matches one pattern component against the entries of a directory.
** Hidden files only match a component that starts with a dot.
*/
static void	glob_scan(t_strv *out, const char *prefix, const char *pattern)
{
	DIR	*dir;
	struct dirent	*entry;
	char	*component;
	size_t	len;

	len = strcspn(pattern, "/");
	component = xstrndup(pattern, len);
	if (*prefix)
		dir = opendir(prefix);
	else
		dir = opendir(".");
	entry = NULL;
	if (dir)
		entry = readdir(dir);
	while (entry)
	{
		if (!str_eq(entry->d_name, ".") && !str_eq(entry->d_name, "..")
			&& (entry->d_name[0] != '.' || component[0] == '.')
			&& pat_match(component, entry->d_name))
			glob_descend(out, xjoin(prefix, entry->d_name), pattern + len);
		entry = readdir(dir);
	}
	if (dir)
		closedir(dir);
	free(component);
}

/*
** Expands the first component of pattern below prefix.
*/
static void	glob_walk(t_strv *out, const char *prefix, const char *pattern)
{
	char	*component;
	char	*literal;
	size_t	len;

	len = strcspn(pattern, "/");
	component = xstrndup(pattern, len);
	if (pat_has_glob(component))
		glob_scan(out, prefix, pattern);
	else
	{
		literal = pat_unescape(component);
		glob_descend(out, xjoin(prefix, literal), pattern + len);
		free(literal);
	}
	free(component);
}

/*
** Pathname expansion: sorted list of the paths matching pattern (empty
** when nothing matches). Quoted characters arrive backslash-escaped.
*/
char	**glob_expand(const char *pattern)
{
	t_strv	out;
	size_t	slashes;

	strv_init(&out);
	slashes = strspn(pattern, "/");
	if (slashes > 0 && pattern[slashes])
		glob_walk(&out, "/", pattern + slashes);
	else if (slashes == 0)
		glob_walk(&out, "", pattern);
	strs_sort(out.v, out.len);
	return (strv_take(&out));
}
