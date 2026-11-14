#include "sh.h"

int	exp_lit_mask(t_exp *e)
{
	if (e->dq || (e->flags & EXP_HEREDOC))
		return (M_QUOTED);
	return (M_LIT);
}

/*
** Backslash: quotes the next character when unquoted; inside double quotes
** and here-documents it only escapes $ ` " \ and newline.
*/
size_t	exp_backslash(t_exp *e, const char *s)
{
	int	heredoc;

	heredoc = e->flags & EXP_HEREDOC;
	if (s[1] == '\n')
		return (2);
	if (!s[1])
	{
		exp_put(e, '\\', M_QUOTED);
		return (1);
	}
	if (!e->dq && !heredoc)
	{
		exp_put(e, s[1], M_QUOTED);
		return (2);
	}
	if (s[1] == '$' || s[1] == '`' || s[1] == '\\'
		|| (s[1] == '"' && !heredoc))
	{
		exp_put(e, s[1], M_QUOTED);
		return (2);
	}
	exp_put(e, '\\', M_QUOTED);
	return (1);
}

size_t	exp_squote(t_exp *e, const char *s)
{
	int	len;
	int	i;

	len = scan_squote(s);
	if (len < 0)
	{
		exp_put(e, '\'', M_QUOTED);
		return (1);
	}
	exp_put(e, '\'', M_MARK);
	i = 1;
	while (i < len - 1)
		exp_put(e, s[i++], M_QUOTED);
	return (len);
}

/*
** `command`: inside backquotes a backslash only escapes $ ` and \.
*/
size_t	exp_backtick(t_exp *e, const char *s)
{
	t_buf	cmd;
	char	*out;
	int	i;

	if (scan_backtick(s) < 0)
	{
		exp_put(e, '`', exp_lit_mask(e));
		return (1);
	}
	buf_init(&cmd);
	buf_adds(&cmd, "");
	i = 1;
	while (s[i] != '`')
	{
		if (s[i] == '\\' && ft_strchr("$`\\", s[i + 1]))
			i++;
		buf_addc(&cmd, s[i++]);
	}
	out = cmdsub_run(cmd.s);
	if (!out)
		e->err = 1;
	exp_puts(e, out, exp_mask(e));
	free(out);
	buf_free(&cmd);
	return (i + 1);
}
