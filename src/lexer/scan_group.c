#include "sh.h"

/*
** s points at '$' or '`': measures $(...), $((...)), ${...} or `...`.
** A lone '$' counts as one ordinary character.
*/
int	scan_subst(const char *s)
{
	int	len;

	if (s[0] == '`')
		return (scan_backtick(s));
	if (s[1] != '(' && s[1] != '{')
		return (1);
	len = scan_group(s + 1);
	if (len < 0)
		return (-1);
	return (len + 1);
}

/*
** s points at '(' or '{': returns the length up to and including the
** matching closer, skipping over nested quotes and substitutions.
*/
int	scan_group(const char *s)
{
	int	i;
	int	len;
	int	depth;
	char	closer;

	closer = ')';
	if (s[0] == '{')
		closer = '}';
	depth = 1;
	i = 1;
	len = 1;
	while (s[i] && depth > 0 && len > 0)
	{
		len = 1;
		if (s[i] == '(' && s[0] == '(')
			depth++;
		else if (s[i] == closer)
			depth--;
		else
			len = scan_unit(s + i);
		i += len;
	}
	if (depth > 0 || len < 0)
		return (-1);
	return (i);
}
