/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_type.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int is_builtin_name(const char *name) {
  static const char *builtins[] = {"cd",     "echo",  "exit", "type",
                                   "export", "unset", "set",  "jobs",
                                   "fg",     "bg",    NULL};
  int i;

  i = 0;
  while (builtins[i]) {
    if (ft_strncmp(name, builtins[i], ft_strlen(builtins[i]) + 1) == 0)
      return (1);
    i++;
  }
  return (0);
}

int builtin_type(char **argv, t_shell *shell) {
  char *path;
  int ret;
  int i;

  ret = 0;
  i = 1;
  while (argv[i]) {
    if (is_builtin_name(argv[i])) {
      ft_putstr_fd(argv[i], 1);
      ft_putendl_fd(" is a shell builtin", 1);
    } else {
      path = resolve_path(argv[i], shell->env);
      if (path) {
        ft_putstr_fd(argv[i], 1);
        ft_putstr_fd(" is ", 1);
        ft_putendl_fd(path, 1);
        free(path);
      } else {
        ft_putstr_fd("42sh: type: ", 2);
        ft_putstr_fd(argv[i], 2);
        ft_putendl_fd(": not found", 2);
        ret = 1;
      }
    }
    i++;
  }
  return (ret);
}
