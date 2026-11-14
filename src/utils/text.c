#include "sh.h"

int	str_eq(const char *a, const char *b)
{
	return (a && b && ft_strcmp(a, b) == 0);
}

/*
** Length of the variable name at the start of s (0 if there is none).
*/
size_t	name_len(const char *s)
{
	size_t	i;

	if (!ft_isalpha(s[0]) && s[0] != '_')
		return (0);
	i = 1;
	while (ft_isalnum(s[i]) || s[i] == '_')
		i++;
	return (i);
}

int	is_name(const char *s)
{
	size_t	len;

	len = name_len(s);
	return (len > 0 && s[len] == '\0');
}

/*
** Strict decimal conversion: the whole string must be a number.
*/
int	parse_long(const char *s, long *out)
{
	char	*end;
	long	value;

	errno = 0;
	value = strtol(s, &end, 10);
	if (end == s || errno != 0)
		return (0);
	while (*end == ' ' || *end == '\t')
		end++;
	if (*end != '\0')
		return (0);
	*out = value;
	return (1);
}

/*
** Shell exit status for a status returned by waitpid.
*/
int	status_code(int wstatus)
{
	if (WIFEXITED(wstatus))
		return (WEXITSTATUS(wstatus));
	if (WIFSIGNALED(wstatus))
		return (128 + WTERMSIG(wstatus));
	if (WIFSTOPPED(wstatus))
		return (128 + WSTOPSIG(wstatus));
	return (1);
}
