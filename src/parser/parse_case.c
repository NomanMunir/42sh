#include "sh.h"

static void	case_patterns(t_parser *p, t_strv *patterns)
{
	if (p->tok->type == T_LPAREN)
		p_next(p);
	while (p->tok->type == T_WORD)
	{
		strv_push(patterns, xstrdup(p->tok->str));
		p_next(p);
		if (p->tok->type != T_PIPE)
			break ;
		p_next(p);
	}
}

/*
** case_item : ['('] pattern ('|' pattern)* ')' [list] (';;' | before esac)
*/
static t_node	*parse_case_item(t_parser *p)
{
	t_node	*item;
	t_strv	patterns;

	item = node_new(N_CASEITEM);
	strv_init(&patterns);
	case_patterns(p, &patterns);
	if (patterns.len == 0 || p->tok->type != T_RPAREN)
		p_error(p);
	item->words = strv_take(&patterns);
	if (p->status != PARSE_OK)
		return (item);
	p_next(p);
	item->left = parse_list(p);
	if (p->tok->type == T_DSEMI)
		p_next(p);
	else if (!p_is_word(p, "esac"))
		p_error(p);
	p_skip_newlines(p);
	return (item);
}

/*
** case : 'case' WORD 'in' case_item* 'esac'
*/
t_node	*parse_case(t_parser *p)
{
	t_node	*n;
	t_node	**link;
	t_strv	words;

	n = node_new(N_CASE);
	strv_init(&words);
	p_next(p);
	if (p->tok->type != T_WORD)
		p_error(p);
	else
		strv_push(&words, xstrdup(p->tok->str));
	n->words = strv_take(&words);
	p_next(p);
	p_skip_newlines(p);
	p_expect(p, "in");
	p_skip_newlines(p);
	link = &n->left;
	while (p->status == PARSE_OK && !p_is_word(p, "esac"))
	{
		*link = parse_case_item(p);
		link = &(*link)->next;
	}
	p_expect(p, "esac");
	return (n);
}
