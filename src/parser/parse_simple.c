#include "sh.h"

/*
** redirection : [io_number] operator WORD
*/
void	parse_redir(t_parser *p, t_node *n)
{
	t_token	*op;
	t_redir	*redir;
	t_redir	**link;

	op = p->tok;
	p_next(p);
	if (p->tok->type != T_WORD)
	{
		p_error(p);
		return ;
	}
	redir = xcalloc(1, sizeof(t_redir));
	redir->op = op->type;
	redir->fd = op->fd;
	redir->word = xstrdup(p->tok->str);
	redir->quoted = op->hd_quoted;
	if (op->heredoc)
		redir->body = xstrdup(op->heredoc);
	link = &n->redirs;
	while (*link)
		link = &(*link)->next;
	*link = redir;
	p_next(p);
}

static int	is_assignment(const char *word)
{
	size_t	len;

	len = name_len(word);
	return (len > 0 && word[len] == '=');
}

/*
** Handles one WORD of a simple command: leading NAME=value words are
** assignments, the first other word may still be an alias.
*/
static void	simple_word(t_parser *p, t_strv *words, t_strv *assigns)
{
	if (words->len == 0 && is_assignment(p->tok->str))
		strv_push(assigns, xstrdup(p->tok->str));
	else if (words->len == 0 && alias_expand_at(p->tok))
		return ;
	else if ((p->tok->flags & TF_ALIASABLE) && alias_expand_at(p->tok))
		return ;
	else
		strv_push(words, xstrdup(p->tok->str));
	p_next(p);
}

/*
** simple_command : (assignment | redirection)* (WORD | redirection)*
*/
t_node	*parse_simple(t_parser *p)
{
	t_node	*n;
	t_strv	words;
	t_strv	assigns;

	n = node_new(N_CMD);
	strv_init(&words);
	strv_init(&assigns);
	while (p->status == PARSE_OK)
	{
		if (is_redir(p->tok->type))
			parse_redir(p, n);
		else if (p->tok->type == T_WORD)
			simple_word(p, &words, &assigns);
		else
			break ;
	}
	n->words = strv_take(&words);
	n->assigns = strv_take(&assigns);
	return (n);
}
