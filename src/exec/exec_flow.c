#include "sh.h"

int	exec_if(t_node *n, int flags)
{
	int	status;

	status = exec_node(n->left, 0);
	if (sh()->unwind)
		return (status);
	if (status == 0)
		return (exec_node(n->right, flags));
	return (exec_node(n->other, flags));
}

/*
** while / until loops.
*/
int	exec_loop(t_node *n, int flags)
{
	int	status;
	int	cond;

	(void)flags;
	status = 0;
	sh()->loop_depth++;
	while (!sh()->unwind)
	{
		cond = exec_node(n->left, 0);
		if (sh()->unwind || (cond == 0) == (n->type == N_UNTIL))
			break ;
		status = exec_node(n->right, 0);
		if (loop_control())
			break ;
	}
	sh()->loop_depth--;
	return (status);
}

int	exec_for(t_node *n, int flags)
{
	char	**list;
	int	status;
	int	i;

	(void)flags;
	list = expand_words(n->words + 1);
	procsub_close();
	if (!list)
		return (1);
	status = 0;
	i = 0;
	sh()->loop_depth++;
	while (list[i] && !sh()->unwind)
	{
		var_set(n->words[0], list[i++], 0);
		status = exec_node(n->right, 0);
		if (loop_control())
			break ;
	}
	sh()->loop_depth--;
	free_strs(list);
	return (status);
}

static int	case_matches(t_node *item, const char *subject)
{
	char	*pattern;
	int	match;
	int	i;

	match = 0;
	i = 0;
	while (item->words[i] && !match)
	{
		pattern = expand_single(item->words[i++], EXP_PATTERN | EXP_TILDE);
		if (pattern)
			match = pat_match(pattern, subject);
		free(pattern);
	}
	return (match);
}

int	exec_case(t_node *n, int flags)
{
	char	*subject;
	t_node	*item;

	subject = expand_single(n->words[0], EXP_TILDE);
	if (!subject)
		return (1);
	item = n->left;
	while (item && !case_matches(item, subject))
		item = item->next;
	free(subject);
	if (!item)
		return (0);
	return (exec_node(item->left, flags));
}
