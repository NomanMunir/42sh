#include "sh.h"

t_token	*tok_add(t_lexer *lx, t_tok type, const char *str)
{
	t_token	*tok;

	tok = xcalloc(1, sizeof(t_token));
	tok->type = type;
	tok->fd = -1;
	if (str)
		tok->str = xstrdup(str);
	if (lx->tail)
		lx->tail->next = tok;
	else
		lx->head = tok;
	lx->tail = tok;
	return (tok);
}

void	tok_free(t_token *tok)
{
	free(tok->str);
	free(tok->heredoc);
	free(tok->chain);
	free(tok);
}

void	tok_free_list(t_token *tok)
{
	t_token	*next;

	while (tok)
	{
		next = tok->next;
		tok_free(tok);
		tok = next;
	}
}

int	is_redir(t_tok type)
{
	return (type >= T_LESS);
}

/*
** Rebuilds a readable command line from tokens [tok, end): this is the
** text shown by jobs, fg and bg.
*/
char	*tokens_text(t_token *tok, t_token *end)
{
	t_buf	buf;
	t_token	*prev;

	buf_init(&buf);
	prev = NULL;
	while (tok && tok != end && tok->type != T_EOF)
	{
		if (prev && prev->type != T_LESSAND && prev->type != T_GREATAND
			&& tok->type != T_SEMI && tok->type != T_NEWLINE)
			buf_addc(&buf, ' ');
		if (tok->fd >= 0)
			buf_addnum(&buf, tok->fd);
		if (tok->type == T_NEWLINE)
			buf_addc(&buf, ';');
		else
			buf_adds(&buf, tok->str);
		prev = tok;
		tok = tok->next;
	}
	return (buf_take(&buf));
}
