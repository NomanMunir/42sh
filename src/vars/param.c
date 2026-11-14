#include "sh.h"

/*
** Installs a new set of positional parameters and returns the previous
** one (owned by the caller).
*/
char	**params_swap(char **params)
{
	char	**old;

	old = sh()->pos;
	sh()->pos = params;
	sh()->npos = strs_len(params);
	return (old);
}

char	*params_join(void)
{
	return (strs_join(sh()->pos, " "));
}

static char	*param_special(char c)
{
	t_shell	*s;

	s = sh();
	if (c == '?')
		return (xitoa(s->status));
	if (c == '$')
		return (xitoa(s->pid));
	if (c == '#')
		return (xitoa(s->npos));
	if (c == '0')
		return (xstrdup(s->name));
	if (c == '-' && s->interactive)
		return (xstrdup("i"));
	if (c == '-')
		return (xstrdup(""));
	if (c == '!' && s->last_bg)
		return (xitoa(s->last_bg));
	if (c == '!')
		return (NULL);
	return (params_join());
}

/*
** Value of a parameter as a fresh string, NULL when it is unset. Handles
** the special parameters, positional parameters and variables.
*/
char	*param_value(const char *name)
{
	char	*value;
	long	index;

	if (name[0] && !name[1] && ft_strchr("?$#0-!@*", name[0]))
		return (param_special(name[0]));
	if (ft_isdigit(name[0]))
	{
		if (!parse_long(name, &index) || index < 1 || index > sh()->npos)
			return (NULL);
		return (xstrdup(sh()->pos[index - 1]));
	}
	value = var_get(name);
	if (!value)
		return (NULL);
	return (xstrdup(value));
}
