#include "sh.h"

/*
** Digits glued to '<' or '>' are the file descriptor of the redirection
** ("2>&1"), not a word of their own.
*/
static int	lex_io_number(t_lexer *lx)
{
	const char	*s;
	int	digits;

	s = lx->src + lx->pos;
	digits = 0;
	while (ft_isdigit(s[digits]))
		digits++;
	if (digits == 0 || digits > 9 || (s[digits] != '<' && s[digits] != '>'))
		return (0);
	lx->pos += digits;
	return (lex_operator(lx, ft_atoi(s)));
}

/*
** Length of the next piece of the word at s: 0 at a metacharacter (the
** word ends), -1 for an unterminated quote or substitution.
*/
static int	word_unit(const char *s, int at_start)
{
	int	len;

	if (at_start && (*s == '<' || *s == '>') && s[1] == '(')
	{
		len = scan_group(s + 1);
		if (len < 0)
			return (-1);
		return (len + 1);
	}
	if (ft_strchr(" \t\n|&;<>()", *s))
		return (0);
	return (scan_unit(s));
}

static void	lex_unterminated(t_lexer *lx)
{
	if (!lx->eof_ok)
	{
		lx->status = PARSE_MORE;
		return ;
	}
	dprintf(STDERR_FILENO,
		"42sh: syntax error: unexpected end of file (unclosed quote)\n");
	lx->status = PARSE_ERR;
}

void	lex_word(t_lexer *lx)
{
	size_t	start;
	int	len;
	t_token	*tok;

	start = lx->pos;
	if (lex_io_number(lx))
		return ;
	len = 1;
	while (lx->src[lx->pos] && len > 0)
	{
		len = word_unit(lx->src + lx->pos, lx->pos == start);
		if (len > 0)
			lx->pos += len;
	}
	if (len < 0)
	{
		lex_unterminated(lx);
		return ;
	}
	tok = tok_add(lx, T_WORD, NULL);
	tok->str = xstrndup(lx->src + start, lx->pos - start);
}

/*
** Quote removal without any expansion (here-document delimiters).
*/
char	*unquote(const char *raw)
{
	t_buf	buf;
	char	quote;

	buf_init(&buf);
	quote = 0;
	while (*raw)
	{
		if (!quote && (*raw == '\'' || *raw == '"'))
			quote = *raw;
		else if (quote && *raw == quote)
			quote = 0;
		else if (*raw == '\\' && quote != '\'' && raw[1])
			buf_addc(&buf, *++raw);
		else
			buf_addc(&buf, *raw);
		raw++;
	}
	return (buf_take(&buf));
}
