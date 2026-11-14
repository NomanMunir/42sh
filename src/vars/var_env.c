#include "sh.h"

int	var_exported(const char *name)
{
	t_var	*var;

	var = map_get(&sh()->vars, name);
	return (var && (var->flags & V_EXPORT));
}

/*
** Detached copy of a variable (NULL if unset), used to restore it after a
** temporary assignment such as "HOME=/tmp cd".
*/
t_var	*var_copy(const char *name)
{
	t_var	*var;
	t_var	*copy;

	var = map_get(&sh()->vars, name);
	if (!var)
		return (NULL);
	copy = xcalloc(1, sizeof(t_var));
	copy->flags = var->flags;
	if (var->value)
		copy->value = xstrdup(var->value);
	return (copy);
}

void	var_restore(const char *name, t_var *old)
{
	if (old)
		map_set(&sh()->vars, name, old);
	else
		map_del(&sh()->vars, name);
	if (str_eq(name, "PATH"))
		map_clear(&sh()->hash);
}

void	vars_import(char **envp)
{
	char	*name;
	size_t	len;
	int	i;

	i = 0;
	while (envp && envp[i])
	{
		len = name_len(envp[i]);
		if (len > 0 && envp[i][len] == '=')
		{
			name = xstrndup(envp[i], len);
			var_set(name, envp[i] + len + 1, V_EXPORT);
			free(name);
		}
		i++;
	}
}

/*
** Builds the environment array passed to execve.
*/
char	**vars_envp(void)
{
	char	**keys;
	t_strv	env;
	t_var	*var;
	int	i;

	keys = map_keys(&sh()->vars);
	strv_init(&env);
	i = 0;
	while (keys[i])
	{
		var = map_get(&sh()->vars, keys[i]);
		if ((var->flags & V_EXPORT) && var->value)
			strv_push(&env, xjoin3(keys[i], "=", var->value));
		i++;
	}
	free(keys);
	return (strv_take(&env));
}
