#include "sh.h"

/*
** subshell : '(' list ')'        group : '{' list '}'
*/
static t_node	*parse_grouping(t_parser *p)
{
	t_node	*n;

	if (p->tok->type == T_LPAREN)
	{
		n = node_new(N_SUBSHELL);
		p_next(p);
		n->left = parse_body(p);
		if (p->tok->type == T_RPAREN)
			p_next(p);
		else
			p_error(p);
		return (n);
	}
	n = node_new(N_GROUP);
	p_next(p);
	n->left = parse_body(p);
	p_expect(p, "}");
	return (n);
}

/*
** function : NAME '(' ')' linebreak command
*/
static t_node	*parse_function(t_parser *p)
{
	t_node	*n;
	t_strv	words;

	n = node_new(N_FUNC);
	strv_init(&words);
	strv_push(&words, xstrdup(p->tok->str));
	n->words = strv_take(&words);
	p_next(p);
	p_next(p);
	p_next(p);
	p_skip_newlines(p);
	n->left = parse_command(p);
	if (!n->left)
		p_error(p);
	return (n);
}

static int	is_function_def(t_token *tok)
{
	return (tok->type == T_WORD && is_name(tok->str)
		&& !is_reserved(tok->str) && tok->next->type == T_LPAREN
		&& tok->next->next->type == T_RPAREN);
}

/*
** Compound commands may be followed by redirections.
*/
t_node	*parse_compound(t_parser *p)
{
	t_node	*n;

	if (p->tok->type == T_LPAREN || p_is_word(p, "{"))
		n = parse_grouping(p);
	else if (p_is_word(p, "if"))
		n = parse_if(p);
	else if (p_is_word(p, "for"))
		n = parse_for(p);
	else if (p_is_word(p, "case"))
		n = parse_case(p);
	else
		n = parse_loop(p);
	while (p->status == PARSE_OK && is_redir(p->tok->type))
		parse_redir(p, n);
	return (n);
}

/*
** command : compound_command | function_definition | simple_command
** Aliases are substituted here, in command position, before anything else
** is decided.
*/
t_node	*parse_command(t_parser *p)
{
	static const char	*openers[] = {"{", "if", "for", "case", "while",
		"until", NULL};
	t_node	*n;
	int	i;

	if (p->status != PARSE_OK)
		return (NULL);
	while (alias_expand_at(p->tok))
		;
	i = 0;
	while (openers[i] && !p_is_word(p, openers[i]))
		i++;
	n = NULL;
	if (++p->depth > PARSE_MAX_DEPTH)
		p_error(p);
	else if (p->tok->type == T_LPAREN || openers[i])
		n = parse_compound(p);
	else if (is_function_def(p->tok))
		n = parse_function(p);
	else if (p->tok->type == T_WORD || is_redir(p->tok->type))
		n = parse_simple(p);
	else
		p_error(p);
	p->depth--;
	return (n);
}
