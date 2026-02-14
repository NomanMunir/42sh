/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int get_var_name_len(const char *s) {
  int i;

  i = 0;
  while (s[i] && (ft_isalnum(s[i]) || s[i] == '_'))
    i++;
  return (i);
}

static char *append_char(char *str, char c) {
  char buf[2];
  char *result;

  buf[0] = c;
  buf[1] = '\0';
  result = ft_strjoin(str, buf);
  free(str);
  return (result);
}

static char *append_str(char *str, const char *s) {
  char *result;

  if (!s)
    return (str);
  result = ft_strjoin(str, s);
  free(str);
  return (result);
}

static char *expand_dollar(const char *word, int *i, t_shell *shell) {
  char *name;
  char *value;
  int len;

  (*i)++;
  if (word[*i] == '{') {
    (*i)++;
    if (word[*i] == '?') {
      (*i)++;
      if (word[*i] == '}')
        (*i)++;
      return (ft_itoa(shell->exit_code));
    }
    len = get_var_name_len(word + *i);
    name = ft_substr(word, *i, len);
    *i += len;
    if (word[*i] == '}')
      (*i)++;
  } else if (word[*i] == '?') {
    (*i)++;
    return (ft_itoa(shell->exit_code));
  } else {
    len = get_var_name_len(word + *i);
    if (len == 0)
      return (ft_strdup("$"));
    name = ft_substr(word, *i, len);
    *i += len;
  }
  value = var_get(shell, name);
  free(name);
  if (value)
    return (ft_strdup(value));
  return (ft_strdup(""));
}

char *expand_variables(const char *word, t_shell *shell) {
  char *result;
  char *expanded;
  int i;

  result = ft_strdup("");
  i = 0;
  while (word[i]) {
    if (word[i] == '$' && word[i + 1]) {
      expanded = expand_dollar(word, &i, shell);
      result = append_str(result, expanded);
      free(expanded);
    } else
      result = append_char(result, word[i++]);
  }
  return (result);
}

void expand_argv(t_node *node, t_shell *shell) {
  int i;
  char *expanded;

  if (!node || !node->argv)
    return;
  i = 0;
  while (node->argv[i]) {
    expanded = expand_variables(node->argv[i], shell);
    free(node->argv[i]);
    node->argv[i] = expanded;
    i++;
  }
}
