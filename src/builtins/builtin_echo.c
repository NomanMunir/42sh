/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int builtin_echo(char **argv, t_shell *shell) {
  int i;
  int newline;

  (void)shell;
  i = 1;
  newline = 1;
  if (argv[i] && ft_strncmp(argv[i], "-n", 3) == 0) {
    newline = 0;
    i++;
  }
  while (argv[i]) {
    ft_putstr_fd(argv[i], 1);
    if (argv[i + 1])
      ft_putchar_fd(' ', 1);
    i++;
  }
  if (newline)
    ft_putchar_fd('\n', 1);
  return (0);
}
