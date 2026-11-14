#include "sh.h"

/*
** Length of the parameter name at s: one special character or digit, or a
** variable name. 0 means the '$' in front of it is literal.
*/
int	param_name_len(const char *s)
{
	if (*s && ft_strchr("?$#!@*-", *s))
		return (1);
	if (ft_isdigit(*s))
		return (1);
	return (name_len(s));
}

/*
** $@ and $*: inside double quotes "$@" gives one field per positional
** parameter while "$*" joins them; unquoted, both are split again.
*/
static void	exp_all_params(t_exp *e, char which)
{
	t_shell	*s;
	int	i;

	s = sh();
	i = 0;
	while (i < s->npos)
	{
		if (i > 0 && e->dq && which == '@')
			exp_put(e, ' ', M_SEP);
		else if (i > 0)
			exp_put(e, ' ', exp_mask(e));
		exp_puts(e, s->pos[i], exp_mask(e));
		i++;
	}
	if (s->npos == 0 && e->dq && which == '@')
		e->no_params = 1;
}

void	exp_param(t_exp *e, const char *name)
{
	char	*value;

	if ((name[0] == '@' || name[0] == '*') && !name[1])
	{
		exp_all_params(e, name[0]);
		return ;
	}
	value = param_value(name);
	exp_puts(e, value, exp_mask(e));
	free(value);
}

/*
** $(( expression )) or $( command ): body is the text between the outer
** parentheses.
*/
static void	exp_paren(t_exp *e, const char *body, int len)
{
	char	*inner;
	char	*res;

	inner = xstrndup(body, len);
	if (inner[0] == '(' && scan_group(inner) == len)
	{
		inner[len - 1] = '\0';
		res = arith_expand(inner + 1);
	}
	else
		res = cmdsub_run(inner);
	if (!res)
		e->err = 1;
	exp_puts(e, res, exp_mask(e));
	free(res);
	free(inner);
}

size_t	exp_dollar(t_exp *e, const char *s)
{
	char	*name;
	int	len;

	len = -1;
	if (s[1] == '(' || s[1] == '{')
		len = scan_group(s + 1);
	if (len >= 0 && s[1] == '{')
		exp_brace(e, s + 2, len - 2);
	else if (len >= 0)
		exp_paren(e, s + 2, len - 2);
	if (len >= 0)
		return (len + 1);
	len = param_name_len(s + 1);
	if (len == 0 || s[1] == '(' || s[1] == '{')
	{
		exp_put(e, '$', exp_lit_mask(e));
		return (1);
	}
	name = xstrndup(s + 1, len);
	exp_param(e, name);
	free(name);
	return (len + 1);
}
