#include "sh.h"

static int	prefix_matches(char *value, size_t len, const char *pattern)
{
	char	saved;
	int	match;

	saved = value[len];
	value[len] = '\0';
	match = pat_match(pattern, value);
	value[len] = saved;
	return (match);
}

/*
** ${parameter#pattern} / ${parameter##pattern}: returns the value without
** its shortest (or longest) matching prefix.
*/
static char	*trim_prefix(char *value, const char *pattern, int longest)
{
	size_t	len;
	size_t	i;

	len = ft_strlen(value);
	i = 0;
	while (!longest && i <= len)
	{
		if (prefix_matches(value, i, pattern))
			return (value + i);
		i++;
	}
	i = len + 1;
	while (longest && i-- > 0)
	{
		if (prefix_matches(value, i, pattern))
			return (value + i);
	}
	return (value);
}

/*
** ${parameter%pattern} / ${parameter%%pattern}: cuts the shortest (or
** longest) matching suffix off the value.
*/
static void	trim_suffix(char *value, const char *pattern, int longest)
{
	size_t	len;
	size_t	i;

	len = ft_strlen(value);
	i = 0;
	while (longest && i <= len)
	{
		if (pat_match(pattern, value + i))
		{
			value[i] = '\0';
			return ;
		}
		i++;
	}
	i = len + 1;
	while (!longest && i-- > 0)
	{
		if (pat_match(pattern, value + i))
		{
			value[i] = '\0';
			return ;
		}
	}
}

void	exp_trim(t_exp *e, const char *name, const char *op)
{
	char	*value;
	char	*pattern;
	char	*result;
	int	longest;

	longest = (op[1] == op[0]);
	value = param_value(name);
	pattern = expand_single(op + 1 + longest, EXP_PATTERN);
	if (!pattern)
		e->err = 1;
	else if (value)
	{
		result = value;
		if (op[0] == '#')
			result = trim_prefix(value, pattern, longest);
		else
			trim_suffix(value, pattern, longest);
		exp_puts(e, result, exp_mask(e));
	}
	free(value);
	free(pattern);
}
