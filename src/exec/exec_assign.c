#include "sh.h"

/*
** Performs "NAME=value" assignments; flags is 0 or V_EXPORT.
** Returns -1 if a value could not be expanded.
*/
int	assign_all(char **assigns, int flags)
{
	char	*name;
	char	*value;
	size_t	len;
	int	i;

	i = 0;
	while (assigns && assigns[i])
	{
		len = name_len(assigns[i]);
		value = expand_single(assigns[i] + len + 1, EXP_ASSIGN);
		if (!value)
			return (-1);
		name = xstrndup(assigns[i], len);
		var_set(name, value, flags);
		free(name);
		free(value);
		i++;
	}
	return (0);
}

static void	assign_one_temp(t_tmpvar **saved, const char *assign)
{
	t_tmpvar	*entry;
	char	*value;
	size_t	len;

	len = name_len(assign);
	value = expand_single(assign + len + 1, EXP_ASSIGN);
	if (!value)
		return ;
	entry = xcalloc(1, sizeof(t_tmpvar));
	entry->name = xstrndup(assign, len);
	entry->old = var_copy(entry->name);
	entry->next = *saved;
	*saved = entry;
	var_set(entry->name, value, V_EXPORT);
	free(value);
}

/*
** Assignments in front of a builtin or function ("HOME=/tmp cd") only
** last for that command: the previous state is saved, then restored by
** assign_restore.
*/
t_tmpvar	*assign_temp(char **assigns)
{
	t_tmpvar	*saved;
	int	i;

	saved = NULL;
	i = 0;
	while (assigns && assigns[i])
		assign_one_temp(&saved, assigns[i++]);
	return (saved);
}

void	assign_restore(t_tmpvar *saved)
{
	t_tmpvar	*next;

	while (saved)
	{
		next = saved->next;
		var_restore(saved->name, saved->old);
		free(saved->name);
		free(saved);
		saved = next;
	}
}
