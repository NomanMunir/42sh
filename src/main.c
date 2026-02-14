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

static void process_line(const char *line, t_shell *shell) {
  t_token *tokens;
  t_node *ast;

  tokens = lexer(line);
  if (!tokens)
    return;
  ast = parse(tokens);
  if (ast) {
    shell->exit_code = execute_ast(ast, shell);
    node_free(ast);
  }
  token_list_free(tokens);
}

static void shell_loop(t_shell *shell) {
  char *line;

  while (shell->running) {
    job_update_status(shell);
    line = readline(PROMPT);
    if (!line) {
      ft_putendl_fd("exit", STDOUT_FILENO);
      break;
    }
    if (*line) {
      add_history(line);
      process_line(line, shell);
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
