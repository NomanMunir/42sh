/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void print_exports(t_shell *shell) {
  int i;

  i = 0;
  while (shell->env[i]) {
    ft_putstr_fd("declare -x ", 1);
    ft_putendl_fd(shell->env[i], 1);
    i++;
  }
}

int builtin_export(char **argv, t_shell *shell) {
  char *eq;
  char *name;
  char *value;
  int i;

  if (!argv[1]) {
    print_exports(shell);
    return (0);
  }
  i = 1;
  while (argv[i]) {
    eq = ft_strchr(argv[i], '=');
    if (eq) {
      name = ft_substr(argv[i], 0, eq - argv[i]);
      value = ft_strdup(eq + 1);
      env_set(shell, name, value);
      free(name);
      free(value);
    } else {
      value = var_get(shell, argv[i]);
      if (value)
        env_set(shell, argv[i], value);
    }
    i++;
  }
  return (0);
}
