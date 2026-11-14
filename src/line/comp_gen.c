#include "sh.h"

/*
** Generator handed to readline: returns the collected candidates one by
** one (readline frees them), then NULL.
*/
char	*comp_gen_list(const char *text, int state)
{
	t_shell	*s;

	(void)text;
	s = sh();
	if (state == 0)
		s->comp_i = 0;
	if (s->comp_i >= s->comp.len)
		return (NULL);
	return (xstrdup(s->comp.v[s->comp_i++]));
}

/*
** File name generator restricted to directories (arguments of cd).
*/
char	*comp_gen_dir(const char *text, int state)
{
	struct stat	info;
	char	*match;
	char	*path;
	int	is_dir;

	match = rl_filename_completion_function(text, state);
	while (match)
	{
		path = tilde_expand(match);
		is_dir = (stat(path, &info) == 0 && S_ISDIR(info.st_mode));
		free(path);
		if (is_dir)
			return (match);
		free(match);
		match = rl_filename_completion_function(text, 1);
	}
	return (NULL);
}
