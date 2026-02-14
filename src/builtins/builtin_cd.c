/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void update_pwd(t_shell *shell, const char *oldpwd) {
  char cwd[4096];

  if (oldpwd)
    env_set(shell, "OLDPWD", oldpwd);
  if (getcwd(cwd, sizeof(cwd)))
    env_set(shell, "PWD", cwd);
}

int builtin_cd(char **argv, t_shell *shell) {
  char *target;
  char *oldpwd;

  oldpwd = env_get(shell->env, "PWD");
  if (!argv[1] || ft_strncmp(argv[1], "--", 3) == 0) {
    target = env_get(shell->env, "HOME");
    if (!target) {
      ft_putendl_fd("42sh: cd: HOME not set", 2);
      return (1);
    }
  } else if (ft_strncmp(argv[1], "-", 2) == 0) {
    target = env_get(shell->env, "OLDPWD");
    if (!target) {
      ft_putendl_fd("42sh: cd: OLDPWD not set", 2);
      return (1);
    }
    ft_putendl_fd(target, 1);
  } else
    target = argv[1];
  if (chdir(target) != 0) {
    ft_putstr_fd("42sh: cd: ", 2);
    ft_putstr_fd(target, 2);
    ft_putendl_fd(": No such file or directory", 2);
    return (1);
  }
  update_pwd(shell, oldpwd);
  return (0);
}
