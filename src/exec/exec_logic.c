#include "sh.h"

int	exec_seq(t_node *n, int flags)
{
	exec_node(n->left, 0);
	if (sh()->unwind)
		return (sh()->status);
	return (exec_node(n->right, flags));
}

int	exec_and(t_node *n, int flags)
{
	int	status;

	status = exec_node(n->left, 0);
	if (status != 0 || sh()->unwind)
		return (status);
	return (exec_node(n->right, flags));
}

int	exec_or(t_node *n, int flags)
{
	int	status;

	status = exec_node(n->left, 0);
	if (status == 0 || sh()->unwind)
		return (status);
	return (exec_node(n->right, flags));
}

int	exec_not(t_node *n, int flags)
{
	int	status;

	(void)flags;
	status = exec_node(n->left, 0);
	if (sh()->unwind)
		return (status);
	return (status == 0);
}

int	exec_group(t_node *n, int flags)
{
	return (exec_node(n->left, flags));
}
