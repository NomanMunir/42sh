#include "sh.h"

/*
** The scan_* functions measure quoted constructs in raw shell text.
** Each returns the length of the construct starting at s, or -1 when it is
** not terminated (the caller then asks for more input).
*/
int	scan_squote(const char *s)
{
	int	i;

	i = 1;
	while (s[i] && s[i] != '\'')
		i++;
	if (!s[i])
		return (-1);
	return (i + 1);
}

int	scan_dquote(const char *s)
{
	int	i;
	int	len;

	i = 1;
	while (s[i] && s[i] != '"')
	{
		len = 1;
		if (s[i] == '\\' && s[i + 1])
			len = 2;
		else if (s[i] == '$' || s[i] == '`')
			len = scan_subst(s + i);
		if (len < 0)
			return (-1);
		i += len;
	}
	if (!s[i])
		return (-1);
	return (i + 1);
}

int	scan_backtick(const char *s)
{
	int	i;

	i = 1;
	while (s[i] && s[i] != '`')
	{
		if (s[i] == '\\' && s[i + 1])
			i++;
		i++;
	}
	if (!s[i])
		return (-1);
	return (i + 1);
}

/*
** One lexical unit of a word: an escaped character, a quoted string, a
** substitution, or a single ordinary character.
*/
int	scan_unit(const char *s)
{
	if (s[0] == '\\')
	{
		if (!s[1])
			return (-1);
		return (2);
	}
	if (s[0] == '\'')
		return (scan_squote(s));
	if (s[0] == '"')
		return (scan_dquote(s));
	if (s[0] == '$' || s[0] == '`')
		return (scan_subst(s));
	return (1);
}
