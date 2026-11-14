#include "sh.h"

/*
** if : 'if' list 'then' list ('elif' list 'then' list)* ['else' list] 'fi'
** An elif is stored as a nested N_IF in the else branch.
*/
t_node	*parse_if(t_parser *p)
{
	t_node	*n;

	n = node_new(N_IF);
	p_next(p);
	n->left = parse_body(p);
	p_expect(p, "then");
	n->right = parse_body(p);
	if (p->status != PARSE_OK)
		return (n);
	if (p_is_word(p, "elif"))
	{
		n->other = parse_if(p);
		return (n);
	}
	if (p_is_word(p, "else"))
	{
		p_next(p);
		n->other = parse_body(p);
	}
	p_expect(p, "fi");
	return (n);
}

/*
** loop : ('while' | 'until') list 'do' list 'done'
*/
t_node	*parse_loop(t_parser *p)
{
	t_node	*n;

	n = node_new(N_WHILE);
	if (p_is_word(p, "until"))
		n->type = N_UNTIL;
	p_next(p);
	n->left = parse_body(p);
	p_expect(p, "do");
	n->right = parse_body(p);
	p_expect(p, "done");
	return (n);
}

static void	for_words(t_parser *p, t_strv *words)
{
	p_skip_newlines(p);
	if (!p_is_word(p, "in"))
	{
		strv_push(words, xstrdup("\"$@\""));
		return ;
	}
	p_next(p);
	while (p->tok->type == T_WORD)
	{
		strv_push(words, xstrdup(p->tok->str));
		p_next(p);
	}
}

/*
** for : 'for' NAME ['in' WORD*] (';' | newline) 'do' list 'done'
*/
t_node	*parse_for(t_parser *p)
{
	t_node	*n;
	t_strv	words;

	n = node_new(N_FOR);
	strv_init(&words);
	p_next(p);
	if (p->tok->type != T_WORD || !is_name(p->tok->str))
		p_error(p);
	else
	{
		strv_push(&words, xstrdup(p->tok->str));
		p_next(p);
		for_words(p, &words);
		if (p->tok->type == T_SEMI)
			p_next(p);
		p_skip_newlines(p);
		p_expect(p, "do");
		n->right = parse_body(p);
		p_expect(p, "done");
	}
	n->words = strv_take(&words);
	return (n);
}
