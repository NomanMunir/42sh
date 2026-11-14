#include "sh.h"

/*
** Out of input before the delimiter: ask for more lines, or accept the
** body as it is when the input really ended.
*/
static void	body_unfinished(t_lexer *lx, const char *delim)
{
	if (!lx->eof_ok)
	{
		lx->status = PARSE_MORE;
		return ;
	}
	dprintf(STDERR_FILENO, "42sh: warning: here-document delimited by "
		"end-of-file (wanted `%s')\n", delim);
}

/*
** Consumes one line of the source; returns 1 if it is the delimiter,
** otherwise appends it to the body.
*/
static int	body_line(t_lexer *lx, t_buf *body, const char *delim,
	int strip)
{
	const char	*line;
	size_t	len;

	line = lx->src + lx->pos;
	len = strcspn(line, "\n");
	lx->pos += len;
	if (lx->src[lx->pos] == '\n')
		lx->pos++;
	while (strip && len > 0 && *line == '\t')
	{
		line++;
		len--;
	}
	if (len == ft_strlen(delim) && ft_strncmp(line, delim, len) == 0)
		return (1);
	buf_addn(body, line, len);
	buf_addc(body, '\n');
	return (0);
}

static void	read_body(t_lexer *lx, t_token *op)
{
	char	*delim;
	t_buf	body;
	int	done;

	delim = unquote(op->next->str);
	op->hd_quoted = (ft_strlen(delim) != ft_strlen(op->next->str));
	buf_init(&body);
	done = 0;
	while (!done && lx->status == PARSE_OK)
	{
		if (!lx->src[lx->pos])
		{
			body_unfinished(lx, delim);
			break ;
		}
		done = body_line(lx, &body, delim, op->type == T_DLESSDASH);
	}
	op->heredoc = buf_take(&body);
	free(delim);
}

/*
** Called after each newline: reads the bodies of the here-documents whose
** operators appeared on the line that just ended.
*/
void	lex_heredocs(t_lexer *lx)
{
	t_token	*tok;

	tok = lx->hd_from;
	if (!tok)
		tok = lx->head;
	while (tok && lx->status == PARSE_OK)
	{
		if ((tok->type == T_DLESS || tok->type == T_DLESSDASH)
			&& !tok->heredoc && tok->next && tok->next->type == T_WORD)
			read_body(lx, tok);
		tok = tok->next;
	}
	lx->hd_from = lx->tail;
}
