#include "sh.h"

static t_redir	*redir_dup(const t_redir *r)
{
	t_redir	*head;
	t_redir	**link;

	head = NULL;
	link = &head;
	while (r)
	{
		*link = xcalloc(1, sizeof(t_redir));
		(*link)->op = r->op;
		(*link)->fd = r->fd;
		(*link)->quoted = r->quoted;
		(*link)->word = xstrdup(r->word);
		if (r->body)
			(*link)->body = xstrdup(r->body);
		link = &(*link)->next;
		r = r->next;
	}
	return (head);
}

/*
** Deep copy of a tree: function bodies are stored and run as copies so
** that a function may redefine or unset itself while running.
*/
t_node	*node_dup(const t_node *n)
{
	t_node	*copy;

	if (!n)
		return (NULL);
	copy = node_new(n->type);
	if (n->words)
		copy->words = strs_dup(n->words);
	if (n->assigns)
		copy->assigns = strs_dup(n->assigns);
	copy->redirs = redir_dup(n->redirs);
	copy->left = node_dup(n->left);
	copy->right = node_dup(n->right);
	copy->other = node_dup(n->other);
	copy->next = node_dup(n->next);
	if (n->text)
		copy->text = xstrdup(n->text);
	return (copy);
}
