#include "sh.h"

/*
** Longest-match operator recognition. fd is the io number that preceded
** the operator (-1 if none). "<(" and ">(" are left to the word lexer:
** they start a process substitution.
*/
int	lex_operator(t_lexer *lx, int fd)
{
	static const char	*ops[] = {"<<-", "&&", "||", ";;", "<<", ">>", "<&",
		">&", "<>", ">|", "|", "&", ";", "(", ")", "<", ">", NULL};
	static const t_tok	types[] = {T_DLESSDASH, T_AND_IF, T_OR_IF, T_DSEMI,
		T_DLESS, T_DGREAT, T_LESSAND, T_GREATAND, T_LESSGREAT, T_CLOBBER,
		T_PIPE, T_AMP, T_SEMI, T_LPAREN, T_RPAREN, T_LESS, T_GREAT};
	const char	*s;
	int	i;

	s = lx->src + lx->pos;
	if ((s[0] == '<' || s[0] == '>') && s[1] == '(' && fd < 0)
		return (0);
	i = 0;
	while (ops[i] && ft_strncmp(s, ops[i], ft_strlen(ops[i])) != 0)
		i++;
	if (!ops[i])
		return (0);
	tok_add(lx, types[i], ops[i])->fd = fd;
	lx->pos += ft_strlen(ops[i]);
	return (1);
}

static void	lex_next(t_lexer *lx)
{
	const char	*s;

	s = lx->src + lx->pos;
	if (*s == ' ' || *s == '\t')
		lx->pos++;
	else if (*s == '\\' && s[1] == '\n')
		lx->pos += 2;
	else if (*s == '#')
		lx->pos += strcspn(s, "\n");
	else if (*s == '\n')
	{
		lx->pos++;
		tok_add(lx, T_NEWLINE, "\n");
		lex_heredocs(lx);
	}
	else if (!lex_operator(lx, -1))
		lex_word(lx);
}

/*
** True when the text ends with an unescaped backslash-newline: the logical
** line continues on the next physical line.
*/
static int	ends_with_continuation(const char *src)
{
	size_t	len;
	size_t	slashes;

	len = ft_strlen(src);
	if (len < 2 || src[len - 1] != '\n')
		return (0);
	slashes = 0;
	while (slashes < len - 1 && src[len - 2 - slashes] == '\\')
		slashes++;
	return (slashes % 2 == 1);
}

/*
** Splits src into tokens. status receives PARSE_OK, PARSE_MORE (the text is
** incomplete: open quote, missing here-document...) or PARSE_ERR. With
** eof_ok set there is no more input to ask for, so PARSE_MORE is never
** returned.
*/
t_token	*lex(const char *src, int eof_ok, int *status)
{
	t_lexer	lx;

	ft_bzero(&lx, sizeof(lx));
	lx.src = src;
	lx.eof_ok = eof_ok;
	while (lx.status == PARSE_OK && src[lx.pos])
		lex_next(&lx);
	if (lx.status == PARSE_OK && (!lx.tail || lx.tail->type != T_NEWLINE))
	{
		tok_add(&lx, T_NEWLINE, "\n");
		lex_heredocs(&lx);
	}
	if (lx.status == PARSE_OK && !eof_ok && ends_with_continuation(src))
		lx.status = PARSE_MORE;
	tok_add(&lx, T_EOF, NULL);
	*status = lx.status;
	return (lx.head);
}
