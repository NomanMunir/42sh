#include "sh.h"

/*
** Length of the bracket expression at p ("[...]"), 0 if it is not closed
** (the '[' is then an ordinary character).
*/
static int	bracket_len(const char *p)
{
	int	i;

	i = 1;
	if (p[i] == '!' || p[i] == '^')
		i++;
	if (p[i] == ']')
		i++;
	while (p[i] && p[i] != ']')
	{
		if (p[i] == '\\' && p[i + 1])
			i++;
		i++;
	}
	if (!p[i])
		return (0);
	return (i + 1);
}

/*
** Does c belong to the bracket expression p of length len? Supports
** lists, ranges (a-z), negation (! or ^) and backslash escapes.
*/
static int	bracket_has(const char *p, int len, unsigned char c)
{
	int	i;
	int	negated;
	int	found;
	unsigned char	low;

	negated = (p[1] == '!' || p[1] == '^');
	i = 1 + negated;
	found = 0;
	while (i < len - 1)
	{
		if (p[i] == '\\' && i + 1 < len - 1)
			i++;
		low = p[i++];
		if (p[i] == '-' && i + 1 < len - 1)
		{
			i++;
			if (p[i] == '\\' && i + 1 < len - 1)
				i++;
			found |= (c >= low && c <= (unsigned char)p[i++]);
		}
		else
			found |= (c == low);
	}
	return (found != negated);
}

/*
** Tries to match one character against the pattern element at p. Returns
** the length of that element, or 0 if c does not match it.
*/
static int	match_one(const char *p, char c)
{
	int	len;

	if (*p == '?')
		return (1);
	if (*p == '[')
	{
		len = bracket_len(p);
		if (len > 0 && bracket_has(p, len, (unsigned char)c))
			return (len);
		if (len > 0)
			return (0);
	}
	if (*p == '\\' && p[1])
		return (2 * (p[1] == c));
	return (*p == c);
}

/*
** Shell pattern matching: * ? [...] and backslash escapes. Iterative with
** a single backtracking point, so it stays linear-ish on hostile input.
*/
int	pat_match(const char *pat, const char *str)
{
	const char	*star_pat;
	const char	*star_str;
	int	len;

	star_pat = NULL;
	star_str = NULL;
	while (*str)
	{
		if (*pat == '*')
		{
			star_pat = ++pat;
			star_str = str;
			continue ;
		}
		len = match_one(pat, *str);
		if (len == 0 && !star_pat)
			return (0);
		if (len == 0)
			pat = star_pat;
		if (len == 0)
			str = ++star_str;
		else
			str++;
		pat += len;
	}
	while (*pat == '*')
		pat++;
	return (!*pat);
}

/*
** True if the pattern holds an unescaped * ? or [.
*/
int	pat_has_glob(const char *pat)
{
	while (*pat)
	{
		if (*pat == '\\' && pat[1])
			pat++;
		else if (*pat == '*' || *pat == '?' || *pat == '[')
			return (1);
		pat++;
	}
	return (0);
}
