/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_H
# define SHELL_H

/* ---- System headers ---- */
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>

/* ---- Project headers ---- */
# include "libft.h"

/* ---- Constants ---- */
# define SHELL_NAME    "42sh"
# define PROMPT        "42sh$ "
# define EXIT_SUCCESS  0
# define EXIT_FAILURE  1

/* ---- Shell state ---- */
typedef struct s_shell
{
	char	**env;       /* copy of environment */
	int		exit_code;   /* last command exit code */
	int		running;     /* main loop flag */
}	t_shell;

/* ---- Core functions ---- */
/* init.c */
t_shell	*shell_init(char **envp);
void	shell_destroy(t_shell *shell);

/* env.c */
char	**env_copy(char **envp);
void	env_free(char **env);

/* signals.c */
void	setup_signals(void);

#endif
