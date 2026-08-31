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

static int is_delim(char c, const char *delim) {
  while (*delim) {
    if (c == *delim)
      return (1);
    delim++;
  }
  return (0);
}

char *ft_strtok(char *str, const char *delim) {
  static char *saved;
  char *start;

  if (str)
    saved = str;
  if (!saved)
    return (NULL);
  while (*saved && is_delim(*saved, delim))
    saved++;
  if (!*saved) {
    saved = NULL;
    return (NULL);
  }
  start = saved;
  while (*saved && !is_delim(*saved, delim))
    saved++;
  if (*saved) {
    *saved = '\0';
    saved++;
  } else
    saved = NULL;
  return (start);
}
