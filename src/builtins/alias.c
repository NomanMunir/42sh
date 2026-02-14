/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alias.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static t_alias *alias_find(t_alias *aliases, const char *name) {
  while (aliases) {
    if (ft_strncmp(aliases->name, name, ft_strlen(name) + 1) == 0)
      return (aliases);
    aliases = aliases->next;
  }
  return (NULL);
}

void alias_set(t_shell *shell, const char *name, const char *value) {
  t_alias *alias;
  t_alias *new_alias;

  alias = alias_find(shell->aliases, name);
  if (alias) {
    free(alias->value);
    alias->value = ft_strdup(value);
    return;
  }
  new_alias = ft_calloc(1, sizeof(t_alias));
  if (!new_alias)
    return;
  new_alias->name = ft_strdup(name);
  new_alias->value = ft_strdup(value);
  new_alias->next = shell->aliases;
  shell->aliases = new_alias;
}

void alias_remove(t_shell *shell, const char *name) {
  t_alias *cur;
  t_alias *prev;

  prev = NULL;
  cur = shell->aliases;
  while (cur) {
    if (ft_strncmp(cur->name, name, ft_strlen(name) + 1) == 0) {
      if (prev)
        prev->next = cur->next;
      else
        shell->aliases = cur->next;
      free(cur->name);
      free(cur->value);
      free(cur);
      return;
    }
    prev = cur;
    cur = cur->next;
  }
}

char *alias_expand(const char *input, t_shell *shell) {
  char *first_word;
  char *rest;
  char *result;
  t_alias *alias;
  int i;

  i = 0;
  while (input[i] && input[i] != ' ' && input[i] != '\t')
    i++;
  first_word = ft_substr(input, 0, i);
  alias = alias_find(shell->aliases, first_word);
  free(first_word);
  if (!alias)
    return (ft_strdup(input));
  rest = ft_strdup(input + i);
  result = ft_strjoin(alias->value, rest);
  free(rest);
  return (result);
}

void alias_list_free(t_alias *aliases) {
  t_alias *tmp;

  while (aliases) {
    tmp = aliases->next;
    free(aliases->name);
    free(aliases->value);
    free(aliases);
    aliases = tmp;
  }
}

int builtin_alias(char **argv, t_shell *shell) {
  t_alias *cur;
  char *eq;
  char *name;
  int i;

  if (!argv[1]) {
    cur = shell->aliases;
    while (cur) {
      ft_putstr_fd("alias ", 1);
      ft_putstr_fd(cur->name, 1);
      ft_putstr_fd("='", 1);
      ft_putstr_fd(cur->value, 1);
      ft_putendl_fd("'", 1);
      cur = cur->next;
    }
    return (0);
  }
  i = 1;
  while (argv[i]) {
    eq = ft_strchr(argv[i], '=');
    if (eq) {
      name = ft_substr(argv[i], 0, eq - argv[i]);
      alias_set(shell, name, eq + 1);
      free(name);
    } else {
      cur = alias_find(shell->aliases, argv[i]);
      if (cur) {
        ft_putstr_fd("alias ", 1);
        ft_putstr_fd(cur->name, 1);
        ft_putstr_fd("='", 1);
        ft_putstr_fd(cur->value, 1);
        ft_putendl_fd("'", 1);
      } else {
        ft_putstr_fd("42sh: alias: ", 2);
        ft_putstr_fd(argv[i], 2);
        ft_putendl_fd(": not found", 2);
      }
    }
    i++;
  }
  return (0);
}

int builtin_unalias(char **argv, t_shell *shell) {
  int i;

  if (!argv[1]) {
    ft_putendl_fd("42sh: unalias: usage: unalias name [name ...]", 2);
    return (2);
  }
  i = 1;
  while (argv[i]) {
    alias_remove(shell, argv[i]);
    i++;
  }
  return (0);
}
