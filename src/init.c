/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

t_shell *shell_init(char **envp) {
  t_shell *shell;

  shell = ft_calloc(1, sizeof(t_shell));
  if (!shell)
    return (NULL);
  shell->env = env_copy(envp);
  if (!shell->env) {
    free(shell);
    return (NULL);
  }
  shell->vars = NULL;
  shell->aliases = NULL;
  shell->jobs = NULL;
  shell->job_count = 0;
  shell->exit_code = 0;
  shell->running = 1;
  shell->interactive = isatty(STDIN_FILENO);
  return (shell);
}

void shell_destroy(t_shell *shell) {
  if (!shell)
    return;
  env_free(shell->env);
  var_list_free(shell->vars);
  alias_list_free(shell->aliases);
  job_list_free(shell->jobs);
  if (shell->interactive)
    rl_clear_history();
  free(shell);
}
