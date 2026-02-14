/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_set.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int builtin_set(char **argv, t_shell *shell) {
  t_var *cur;

  (void)argv;
  cur = shell->vars;
  while (cur) {
    ft_putstr_fd(cur->name, 1);
    ft_putchar_fd('=', 1);
    ft_putendl_fd(cur->value, 1);
    cur = cur->next;
  }
  return (0);
}
