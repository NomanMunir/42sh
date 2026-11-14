#include "sh.h"

/*
** Shell variables live in one table; exported ones carry V_EXPORT and form
** the environment of the commands. A variable may be exported without a
** value ("export NAME").
*/
void	var_free(void *var)
{
	free(((t_var *)var)->value);
	free(var);
}

char	*var_get(const char *name)
{
	t_var	*var;

	var = map_get(&sh()->vars, name);
	if (!var)
		return (NULL);
	return (var->value);
}

/*
** Some variables have side effects when they change.
*/
static void	var_changed(const char *name)
{
	if (str_eq(name, "PATH"))
		map_clear(&sh()->hash);
}

/*
** Sets name to value (NULL keeps the current value) and adds flags.
*/
void	var_set(const char *name, const char *value, int flags)
{
	t_var	*var;
	char	*copy;

	var = map_get(&sh()->vars, name);
	if (!var)
	{
		var = xcalloc(1, sizeof(t_var));
		map_set(&sh()->vars, name, var);
	}
	if (value)
	{
		copy = xstrdup(value);
		free(var->value);
		var->value = copy;
	}
	var->flags |= flags;
	var_changed(name);
}

void	var_unset(const char *name)
{
	map_del(&sh()->vars, name);
	var_changed(name);
}
