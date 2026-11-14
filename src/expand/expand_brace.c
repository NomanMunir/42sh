#include "sh.h"

static void	bad_substitution(t_exp *e, const char *body)
{
	dprintf(STDERR_FILENO, "42sh: ${%s}: bad substitution\n", body);
	e->err = 1;
}

/*
** ${#parameter}: length of the value ($# for ${#@} and ${#*}).
*/
static void	exp_length(t_exp *e, const char *name)
{
	char	*value;
	char	*text;
	size_t	len;

	len = 0;
	if ((name[0] == '@' || name[0] == '*') && !name[1])
		len = sh()->npos;
	else
	{
		value = param_value(name);
		if (value)
			len = ft_strlen(value);
		free(value);
	}
	text = xitoa(len);
	exp_puts(e, text, exp_mask(e));
	free(text);
}

/*
** Inside braces a positional parameter may have several digits.
*/
static int	brace_name_len(const char *s)
{
	int	len;

	if (!ft_isdigit(*s))
		return (param_name_len(s));
	len = 0;
	while (ft_isdigit(s[len]))
		len++;
	return (len);
}

/*
** Applies the operator that follows the name: none, :- := :? :+ (and the
** variants without colon), %, %%, # or ##. Returns 0 if it is not valid.
*/
static int	brace_operator(t_exp *e, const char *name, const char *op)
{
	int	colon;

	colon = (op[0] == ':');
	if (!op[0])
		exp_param(e, name);
	else if (op[0] == '%' || op[0] == '#')
		exp_trim(e, name, op);
	else if (op[colon] && ft_strchr("-=?+", op[colon]))
		exp_cond(e, name, op);
	else
		return (0);
	return (1);
}

/*
** ${...}: body is the text between the braces.
*/
void	exp_brace(t_exp *e, const char *body, int len)
{
	char	*text;
	char	*name;
	int	nlen;

	text = xstrndup(body, len);
	if (text[0] == '#' && text[1] && brace_name_len(text + 1) == len - 1)
		exp_length(e, text + 1);
	else
	{
		nlen = brace_name_len(text);
		name = xstrndup(text, nlen);
		if (nlen == 0 || !brace_operator(e, name, text + nlen))
			bad_substitution(e, text);
		free(name);
	}
	free(text);
}
