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

static char *preprocess_line(const char *line, t_shell *shell) {
  char *expanded;
  char *alias_line;

  if (!shell->interactive) {
    alias_line = alias_expand(line, shell);
    return (alias_line);
  }
  expanded = expand_history(line);
  if (!expanded)
    return (NULL);
  alias_line = alias_expand(expanded, shell);
  free(expanded);
  return (alias_line);
}

/*
** shell_loop: one read loop for both modes. read_input_line returns a
** readline-edited line when interactive and a plain stdin line when not,
** so here-documents (collected inside process_input) read from the same
** source the loop does. History and the EOF "exit" echo are interactive
** only.
*/
static void shell_loop(t_shell *shell) {
  char *line;
  char *processed;

  while (shell->running) {
    if (shell->interactive)
      job_update_status(shell);
    line = read_input_line(shell, PROMPT);
    if (!line) {
      if (shell->interactive)
        ft_putendl_fd("exit", STDOUT_FILENO);
      break;
    }
    if (*line) {
      if (shell->interactive)
        add_history(line);
      processed = preprocess_line(line, shell);
      if (processed) {
        process_input(processed, shell);
        free(processed);
      }
    }
    free(line);
  }
}

int main(int argc, char **argv, char **envp) {
  t_shell *shell;
  int exit_code;

  (void)argc;
  (void)argv;
  shell = shell_init(envp);
  if (!shell) {
    ft_putendl_fd("42sh: fatal: initialization failed", STDERR_FILENO);
    return (EXIT_FAILURE);
  }
  setup_signals();
  shell_loop(shell);
  exit_code = shell->exit_code;
  shell_destroy(shell);
  return (exit_code);
}
