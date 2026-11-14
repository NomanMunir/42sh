#include "sh.h"

t_node	*node_new(t_ntype type)
{
	t_node	*n;

	n = xcalloc(1, sizeof(t_node));
	n->type = type;
	return (n);
}

/*
** Binary node; a missing side simply yields the other one.
*/
t_node	*node_join(t_ntype type, t_node *left, t_node *right)
{
	t_node	*n;

	if (!left)
		return (right);
	if (!right)
		return (left);
	n = node_new(type);
	n->left = left;
	n->right = right;
	return (n);
}

t_node	*node_wrap(t_ntype type, t_node *child)
{
	t_node	*n;

	n = node_new(type);
	n->left = child;
	if (child && child->text)
		n->text = xstrdup(child->text);
	return (n);
}

void	redir_free(t_redir *r)
{
	t_redir	*next;

	while (r)
	{
		next = r->next;
		free(r->word);
		free(r->body);
		free(r);
		r = next;
	}
}

void	node_free(t_node *n)
{
	if (!n)
		return ;
	free_strs(n->words);
	free_strs(n->assigns);
	redir_free(n->redirs);
	node_free(n->left);
	node_free(n->right);
	node_free(n->other);
	node_free(n->next);
	free(n->text);
	free(n);
}
