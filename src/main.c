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

static void shell_loop_interactive(t_shell *shell) {
  char *line;
  char *processed;

  while (shell->running) {
    job_update_status(shell);
    line = readline(PROMPT);
    if (!line) {
      ft_putendl_fd("exit", STDOUT_FILENO);
      break;
    }
    if (*line) {
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

static void shell_loop_pipe(t_shell *shell) {
  char *line;
  char *processed;
  char buf[65536];
  int n;

  n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
  if (n <= 0)
    return;
  buf[n] = '\0';
  line = ft_strtok(buf, "\n");
  while (line && shell->running) {
    if (*line) {
      processed = preprocess_line(line, shell);
      if (processed) {
        process_input(processed, shell);
        free(processed);
      }
    }
    line = ft_strtok(NULL, "\n");
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
  if (shell->interactive)
    shell_loop_interactive(shell);
  else
    shell_loop_pipe(shell);
  exit_code = shell->exit_code;
  shell_destroy(shell);
  return (exit_code);
}
