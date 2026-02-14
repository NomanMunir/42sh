/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tilde.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

char *expand_tilde(const char *word, t_shell *shell) {
  char *home;

  if (!word || word[0] != '~')
    return (ft_strdup(word));
  if (word[1] == '\0' || word[1] == '/') {
    home = var_get(shell, "HOME");
    if (!home)
      return (ft_strdup(word));
    if (word[1] == '/')
      return (ft_strjoin(home, word + 1));
    return (ft_strdup(home));
  }
  return (ft_strdup(word));
}

void expand_tilde_argv(t_node *node, t_shell *shell) {
  int i;
  char *expanded;

  if (!node || !node->argv)
    return;
  i = 0;
  while (node->argv[i]) {
    expanded = expand_tilde(node->argv[i], shell);
    free(node->argv[i]);
    node->argv[i] = expanded;
    i++;
  }
}
