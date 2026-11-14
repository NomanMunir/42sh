#include "sh.h"

static void	node_set_text(t_node *n, t_token *start, t_token *end)
{
	if (n && !n->text)
		n->text = tokens_text(start, end);
}

/*
** pipeline : ['!'] command ('|' linebreak command)*
*/
static t_node	*parse_pipeline(t_parser *p)
{
	t_node	*left;
	t_token	*start;
	int	negated;

	negated = 0;
	while (p_is_word(p, "!"))
	{
		negated = !negated;
		p_next(p);
	}
	start = p->tok;
	left = parse_command(p);
	while (p->status == PARSE_OK && p->tok->type == T_PIPE)
	{
		p_next(p);
		p_skip_newlines(p);
		left = node_join(N_PIPE, left, parse_command(p));
	}
	node_set_text(left, start, p->tok);
	if (negated && left)
		left = node_wrap(N_NOT, left);
	return (left);
}

/*
** and_or : pipeline (('&&' | '||') linebreak pipeline)*
*/
static t_node	*parse_and_or(t_parser *p)
{
	t_node	*left;
	t_token	*start;
	t_ntype	type;

	start = p->tok;
	left = parse_pipeline(p);
	while (p->status == PARSE_OK
		&& (p->tok->type == T_AND_IF || p->tok->type == T_OR_IF))
	{
		type = N_AND;
		if (p->tok->type == T_OR_IF)
			type = N_OR;
		p_next(p);
		p_skip_newlines(p);
		left = node_join(type, left, parse_pipeline(p));
		if (left)
		{
			free(left->text);
			left->text = NULL;
		}
		node_set_text(left, start, p->tok);
	}
	return (left);
}

/*
** list : linebreak and_or ((';' | '&' | newline) linebreak and_or)* [sep]
** An and_or followed by '&' runs in the background.
*/
t_node	*parse_list(t_parser *p)
{
	t_node	*list;
	t_node	*item;

	list = NULL;
	p_skip_newlines(p);
	while (p->status == PARSE_OK && !p_at_list_end(p))
	{
		item = parse_and_or(p);
		if (item && p->tok->type == T_AMP)
			item = node_wrap(N_BG, item);
		list = node_join(N_SEQ, list, item);
		if (p->tok->type != T_AMP && p->tok->type != T_SEMI
			&& p->tok->type != T_NEWLINE)
			break ;
		p_next(p);
		p_skip_newlines(p);
	}
	return (list);
}
