#include "sh.h"

/*
** Length of the event designator starting with the '!' at s: !!, !n,
** !-n or !word. 0 when the '!' is an ordinary character.
*/
static size_t	event_len(const char *s)
{
	size_t	len;

	if (s[1] == '!')
		return (2);
	len = 1;
	if (s[len] == '-' && ft_isdigit(s[len + 1]))
		len++;
	if (ft_isdigit(s[len]))
	{
		while (ft_isdigit(s[len]))
			len++;
		return (len);
	}
	len = 1 + strcspn(s + 1, " \t\n;&|()<>\"'`=:!");
	if (len == 1)
		return (0);
	return (len);
}

/*
** Command designated by des (the text after the '!'), NULL if the
** history has no such event.
*/
static const char	*event_find(const char *des, size_t len)
{
	char	*word;
	long	number;
	long	pos;

	if (des[0] == '!')
		return (hist_line(hist_count() - 1));
	word = xstrndup(des, len);
	pos = hist_count() - 1;
	if (parse_long(word, &number) && number < 0)
		pos = hist_count() + number;
	else if (parse_long(word, &number))
		pos = number - hist_number(0);
	else
	{
		while (pos >= 0 && ft_strncmp(hist_line(pos), word, len) != 0)
			pos--;
	}
	free(word);
	if (pos < 0 || pos >= hist_count())
		return (NULL);
	return (hist_line((int)pos));
}

/*
** Expands the event at line[*i] into out. Returns 1 if it did, 0 if the
** '!' is literal, -1 if the event does not exist.
*/
static int	event_expand(t_buf *out, const char *line, size_t *i)
{
	const char	*event;
	size_t	len;

	if (*i > 0 && ft_strchr("[${", line[*i - 1]))
		return (0);
	len = event_len(line + *i);
	if (len == 0)
		return (0);
	event = event_find(line + *i + 1, len - 1);
	if (!event)
	{
		dprintf(STDERR_FILENO, "42sh: %.*s: event not found\n", (int)len,
			line + *i);
		return (-1);
	}
	buf_adds(out, event);
	*i += len;
	return (1);
}

/*
** Handles the character at line[*i], keeping track of the quote it may
** be in. Returns 1 after an expansion, -1 after a failed one.
*/
static int	expand_step(t_buf *out, const char *line, size_t *i, char *quote)
{
	int	done;

	done = 0;
	if (line[*i] == '\\' && line[*i + 1] && *quote != '\'')
		buf_addc(out, line[(*i)++]);
	else if (line[*i] == '!' && *quote != '\'')
		done = event_expand(out, line, i);
	else if (!*quote && (line[*i] == '\'' || line[*i] == '"'))
		*quote = line[*i];
	else if (*quote == line[*i])
		*quote = 0;
	if (done == 0)
		buf_addc(out, line[(*i)++]);
	return (done);
}

/*
** History expansion of a line: !!, !n, !-n, !word. Single quotes and
** backslashes protect the '!'. Returns the new line (*changed tells
** whether anything was replaced) or NULL after an "event not found".
*/
char	*hist_expand(const char *line, int *changed)
{
	t_buf	out;
	size_t	i;
	char	quote;
	int	done;

	buf_init(&out);
	i = 0;
	quote = 0;
	done = 0;
	while (line[i] && done >= 0)
	{
		done = expand_step(&out, line, &i, &quote);
		if (done > 0)
			*changed = 1;
	}
	if (done >= 0)
		return (buf_take(&out));
	buf_free(&out);
	return (NULL);
}
