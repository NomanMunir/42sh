#include "sh.h"

void	exp_init(t_exp *e, int flags)
{
	ft_bzero(e, sizeof(*e));
	e->flags = flags;
}

void	exp_free(t_exp *e)
{
	buf_free(&e->out);
	buf_free(&e->mask);
}

/*
** Expands one word and splits it into fields appended to out.
** Returns -1 if the expansion failed.
*/
static int	expand_word(const char *raw, t_strv *out)
{
	t_exp	e;

	exp_init(&e, EXP_TILDE | EXP_PROCSUB);
	exp_run(&e, raw);
	if (!e.err)
		fields_split(&e, out);
	exp_free(&e);
	return (-e.err);
}

/*
** Full expansion of command words: tilde, parameters, command and
** arithmetic substitution, field splitting, globbing and quote removal.
** Returns a new argv, or NULL after an expansion error.
*/
char	**expand_words(char **raw)
{
	t_strv	out;
	int	i;

	strv_init(&out);
	i = 0;
	while (raw && raw[i])
	{
		if (expand_word(raw[i], &out) < 0)
		{
			strv_free(&out);
			return (NULL);
		}
		i++;
	}
	return (strv_take(&out));
}

/*
** Expansion to exactly one string, without field splitting or globbing
** (assignments, redirection targets, here-documents, case words...).
** With EXP_PATTERN the quoted pattern characters come back escaped.
*/
char	*expand_single(const char *raw, int flags)
{
	t_exp	e;
	t_buf	res;
	size_t	i;

	exp_init(&e, flags);
	exp_run(&e, raw);
	buf_init(&res);
	i = 0;
	while (i < e.out.len && !e.err)
	{
		if ((flags & EXP_PATTERN) && e.mask.s[i] == M_QUOTED
			&& ft_strchr("*?[]\\", e.out.s[i]))
			buf_addc(&res, '\\');
		if (e.mask.s[i] != M_MARK)
			buf_addc(&res, e.out.s[i]);
		i++;
	}
	exp_free(&e);
	if (!e.err)
		return (buf_take(&res));
	buf_free(&res);
	return (NULL);
}
