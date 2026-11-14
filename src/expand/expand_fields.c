#include "sh.h"

/*
** Emits the field being built: the matches of its pattern if it holds
** unquoted glob characters and something matches, else its text.
*/
static void	field_flush(t_field *f)
{
	char	**matches;
	int	i;

	matches = NULL;
	if (f->glob)
		matches = glob_expand(f->pat.s);
	i = 0;
	while (matches && matches[i])
		strv_push(f->out, matches[i++]);
	if (i == 0 && f->txt.s)
		strv_push(f->out, xstrdup(f->txt.s));
	else if (i == 0)
		strv_push(f->out, xstrdup(""));
	free(matches);
	f->txt.len = 0;
	f->pat.len = 0;
	if (f->txt.s)
		f->txt.s[0] = '\0';
	f->active = 0;
	f->glob = 0;
}

/*
** IFS white space only separates fields; any other IFS character
** delimits one, so two of them in a row produce an empty field.
*/
static void	field_delim(t_field *f, char c)
{
	if (c == ' ' || c == '\t' || c == '\n')
	{
		if (f->active)
		{
			field_flush(f);
			f->ws_end = 1;
		}
		return ;
	}
	if (f->active || !f->ws_end)
		field_flush(f);
	f->ws_end = 0;
}

static void	field_char(t_field *f, char c, char mask)
{
	if (mask == M_MARK)
		f->active = 1;
	else if (mask == M_SEP)
	{
		field_flush(f);
		f->active = 1;
	}
	else if (mask == M_EXP && ft_strchr(f->ifs, c))
		field_delim(f, c);
	else
	{
		if (mask == M_QUOTED && ft_strchr("*?[]\\", c))
			buf_addc(&f->pat, '\\');
		if (mask != M_QUOTED && ft_strchr("*?[", c))
			f->glob = 1;
		buf_addc(&f->txt, c);
		buf_addc(&f->pat, c);
		f->active = 1;
		f->ws_end = 0;
	}
}

/*
** Field splitting, pathname expansion and quote removal of an expanded
** word. A word made only of an empty "$@" produces no field at all.
*/
void	fields_split(t_exp *e, t_strv *out)
{
	t_field	f;
	size_t	i;

	ft_bzero(&f, sizeof(f));
	f.out = out;
	f.ifs = var_get("IFS");
	if (!f.ifs)
		f.ifs = " \t\n";
	i = 0;
	while (i < e->out.len)
	{
		field_char(&f, e->out.s[i], e->mask.s[i]);
		i++;
	}
	if (f.active && !(e->no_params && f.txt.len == 0))
		field_flush(&f);
	buf_free(&f.txt);
	buf_free(&f.pat);
}
