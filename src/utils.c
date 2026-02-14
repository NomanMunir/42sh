/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

char **ft_realloc_arr(char **arr, int old_count, int new_cap) {
  char **new_arr;
  int i;

  new_arr = ft_calloc(new_cap + 1, sizeof(char *));
  if (!new_arr)
    return (arr);
  i = 0;
  while (i < old_count && arr[i]) {
    new_arr[i] = arr[i];
    i++;
  }
  free(arr);
  return (new_arr);
}

void process_input(const char *line, t_shell *shell) {
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
