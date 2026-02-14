/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

/*
** Main REPL loop: read input via readline, add to history, and (for now)
** echo back the input as a placeholder for the future lexer/parser/executor.
*/
static void	shell_loop(t_shell *shell)
{
	char	*line;

	while (shell->running)
	{
		line = readline(PROMPT);
		if (!line)
		{
			ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		if (*line)
			add_history(line);
		/* TODO: lexer → parser → executor pipeline */
		free(line);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	*shell;

	(void)argc;
	(void)argv;
	shell = shell_init(envp);
	if (!shell)
	{
		ft_putendl_fd("42sh: fatal: initialization failed", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	setup_signals();
	shell_loop(shell);
	shell_destroy(shell);
	return (shell->exit_code);
}
