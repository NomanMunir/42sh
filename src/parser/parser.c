#include "sh.h"

/*
** Reports a syntax error at the current token. Running out of tokens is
** not an error while more input can still be read: the caller is then
** asked to supply the next line (PARSE_MORE).
*/
void	p_error(t_parser *p)
{
	const char	*text;

	if (p->status != PARSE_OK)
		return ;
	if (p->tok->type == T_EOF && !p->eof_ok)
	{
		p->status = PARSE_MORE;
		return ;
	}
	p->status = PARSE_ERR;
	text = p->tok->str;
	if (p->tok->type == T_NEWLINE)
		text = "newline";
	if (p->tok->type == T_EOF)
		dprintf(STDERR_FILENO,
			"42sh: syntax error: unexpected end of file\n");
	else
		dprintf(STDERR_FILENO,
			"42sh: syntax error near unexpected token `%s'\n", text);
}

void	p_next(t_parser *p)
{
	if (p->tok->type != T_EOF)
		p->tok = p->tok->next;
}

void	p_skip_newlines(t_parser *p)
{
	while (p->tok->type == T_NEWLINE)
		p->tok = p->tok->next;
}

/*
** Parses a complete program text into *out (NULL for an empty text).
** Returns PARSE_OK, PARSE_MORE or PARSE_ERR.
*/
int	parse_text(const char *text, int eof_ok, t_node **out)
{
	t_parser	p;
	t_token	*tokens;

	*out = NULL;
	ft_bzero(&p, sizeof(p));
	p.eof_ok = eof_ok;
	tokens = lex(text, eof_ok, &p.status);
	p.tok = tokens;
	if (p.status == PARSE_OK)
		*out = parse_list(&p);
	if (p.status == PARSE_OK && p.tok->type != T_EOF)
		p_error(&p);
	tok_free_list(tokens);
	if (p.status != PARSE_OK)
	{
		node_free(*out);
		*out = NULL;
	}
	return (p.status);
}
