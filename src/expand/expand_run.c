#include "sh.h"

void	exp_put(t_exp *e, char c, int mask)
{
	buf_addc(&e->out, c);
	buf_addc(&e->mask, (char)mask);
}

void	exp_puts(t_exp *e, const char *s, int mask)
{
	while (s && *s)
		exp_put(e, *s++, mask);
}

/*
** Mask of the text produced by a substitution: it is protected from field
** splitting inside double quotes and here-documents.
*/
int	exp_mask(t_exp *e)
{
	if (e->dq || (e->flags & EXP_HEREDOC))
		return (M_QUOTED);
	return (M_EXP);
}

/*
** Handles the construct at s and returns how many characters it used.
*/
static size_t	exp_one(t_exp *e, const char *s)
{
	int	heredoc;

	heredoc = e->flags & EXP_HEREDOC;
	if (*s == '\\')
		return (exp_backslash(e, s));
	if (*s == '\'' && !e->dq && !heredoc)
		return (exp_squote(e, s));
	if (*s == '"' && !heredoc)
	{
		e->dq = !e->dq;
		exp_put(e, '"', M_MARK);
		return (1);
	}
	if (*s == '$')
		return (exp_dollar(e, s));
	if (*s == '`')
		return (exp_backtick(e, s));
	if (*s == ':' && s[1] == '~' && !e->dq && (e->flags & EXP_ASSIGN))
	{
		exp_put(e, ':', M_LIT);
		return (1 + exp_tilde(e, s + 1));
	}
	exp_put(e, *s, exp_lit_mask(e));
	return (1);
}

/*
** Walks through raw shell text, appending its expansion to e.
*/
void	exp_run(t_exp *e, const char *s)
{
	size_t	i;

	i = 0;
	if (!e->dq && s[0] == '~' && (e->flags & (EXP_TILDE | EXP_ASSIGN)))
		i = exp_tilde(e, s);
	else if (!e->dq && (s[0] == '<' || s[0] == '>') && s[1] == '('
		&& (e->flags & EXP_PROCSUB))
		i = exp_procsub(e, s);
	while (s[i] && !e->err)
		i += exp_one(e, s + i);
}
