/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	env_count(char **envp)
{
	int	i;

	i = 0;
	while (envp && envp[i])
		i++;
	return (i);
}

char	**env_copy(char **envp)
{
	char	**env;
	int		count;
	int		i;

	count = env_count(envp);
	env = ft_calloc(count + 1, sizeof(char *));
	if (!env)
		return (NULL);
	i = 0;
	while (i < count)
	{
		env[i] = ft_strdup(envp[i]);
		if (!env[i])
		{
			env_free(env);
			return (NULL);
		}
		i++;
	}
	return (env);
}

void	env_free(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}
