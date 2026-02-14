/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int is_numeric(const char *s) {
  int i;

  i = 0;
  if (s[i] == '+' || s[i] == '-')
    i++;
  if (!s[i])
    return (0);
  while (s[i]) {
    if (!ft_isdigit(s[i]))
      return (0);
    i++;
  }
  return (1);
}

int builtin_exit(char **argv, t_shell *shell) {
  int exit_code;

  ft_putendl_fd("exit", STDERR_FILENO);
  if (argv[1] && argv[2]) {
    ft_putendl_fd("42sh: exit: too many arguments", 2);
    return (1);
  }
  if (argv[1]) {
    if (!is_numeric(argv[1])) {
      ft_putstr_fd("42sh: exit: ", 2);
      ft_putstr_fd(argv[1], 2);
      ft_putendl_fd(": numeric argument required", 2);
      shell->exit_code = 2;
      shell->running = 0;
      return (2);
    }
    exit_code = ft_atoi(argv[1]);
    shell->exit_code = (unsigned char)exit_code;
  }
  shell->running = 0;
  return (shell->exit_code);
}
