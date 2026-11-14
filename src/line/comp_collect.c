#include "sh.h"

static void	comp_add(const char *text, const char *candidate)
{
	if (ft_strncmp(candidate, text, ft_strlen(text)) == 0)
		strv_push(&sh()->comp, xstrdup(candidate));
}

static void	comp_add_keys(t_map *map, const char *text)
{
	char	**keys;
	int	i;

	keys = map_keys(map);
	i = 0;
	while (keys[i])
		comp_add(text, keys[i++]);
	free(keys);
}

/*
** Executables of one PATH directory whose name starts with text.
*/
static void	comp_add_dir(const char *dir, const char *text)
{
	DIR	*stream;
	struct dirent	*entry;
	char	*path;

	stream = opendir(dir);
	entry = NULL;
	if (stream)
		entry = readdir(stream);
	while (entry)
	{
		if (entry->d_name[0] != '.'
			&& ft_strncmp(entry->d_name, text, ft_strlen(text)) == 0)
		{
			path = xjoin3(dir, "/", entry->d_name);
			if (access(path, X_OK) == 0)
				comp_add(text, entry->d_name);
			free(path);
		}
		entry = readdir(stream);
	}
	if (stream)
		closedir(stream);
}

/*
** Command names: builtins, aliases, functions and the programs found
** in PATH.
*/
static void	comp_add_commands(const char *text)
{
	const char	*dirs;
	char	*dir;
	size_t	len;
	int	i;

	i = 0;
	while (builtin_name(i))
		comp_add(text, builtin_name(i++));
	comp_add_keys(&sh()->aliases, text);
	comp_add_keys(&sh()->funcs, text);
	dirs = var_get("PATH");
	while (dirs && *dirs)
	{
		len = strcspn(dirs, ":");
		dir = xstrndup(dirs, len);
		if (len > 0)
			comp_add_dir(dir, text);
		free(dir);
		dirs += len + (dirs[len] == ':');
	}
}

/*
** Fills the candidate list of the shell with what can complete text in
** the context ctx.
*/
void	comp_collect(int ctx, const char *text)
{
	t_job	*job;
	char	spec[32];

	strv_free(&sh()->comp);
	if (ctx == C_CMD)
		comp_add_commands(text);
	else if (ctx == C_VAR || ctx == C_VARBRACE || ctx == C_VARNAME)
		comp_add_keys(&sh()->vars, text);
	else if (ctx == C_ALIAS)
		comp_add_keys(&sh()->aliases, text);
	else if (ctx == C_SETOPT)
	{
		comp_add(text, "emacs");
		comp_add(text, "vi");
	}
	job = sh()->jobs;
	while (job && ctx == C_JOB)
	{
		snprintf(spec, sizeof(spec), "%%%d", job->id);
		comp_add(text, spec);
		job = job->next;
	}
}
