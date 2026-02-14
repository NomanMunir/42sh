/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

/*
** Dispatch table for built-in commands.
** Returns -1 if the command is not a built-in, otherwise returns the exit code.
*/
int run_builtin(char **argv, t_shell *shell) {
  if (!argv || !argv[0])
    return (-1);
  if (ft_strncmp(argv[0], "cd", 3) == 0)
    return (builtin_cd(argv, shell));
  if (ft_strncmp(argv[0], "echo", 5) == 0)
    return (builtin_echo(argv, shell));
  if (ft_strncmp(argv[0], "exit", 5) == 0)
    return (builtin_exit(argv, shell));
  if (ft_strncmp(argv[0], "type", 5) == 0)
    return (builtin_type(argv, shell));
  if (ft_strncmp(argv[0], "export", 7) == 0)
    return (builtin_export(argv, shell));
  if (ft_strncmp(argv[0], "unset", 6) == 0)
    return (builtin_unset(argv, shell));
  if (ft_strncmp(argv[0], "set", 4) == 0)
    return (builtin_set(argv, shell));
  if (ft_strncmp(argv[0], "jobs", 5) == 0)
    return (builtin_jobs(argv, shell));
  if (ft_strncmp(argv[0], "fg", 3) == 0)
    return (builtin_fg(argv, shell));
  if (ft_strncmp(argv[0], "bg", 3) == 0)
    return (builtin_bg(argv, shell));
  return (-1);
}
