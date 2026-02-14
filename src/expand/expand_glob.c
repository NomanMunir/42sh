/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_glob.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <dirent.h>

static int has_glob_char(const char *s) {
  while (*s) {
    if (*s == '*' || *s == '?' || *s == '[')
      return (1);
    s++;
  }
  return (0);
}

static int match_bracket(const char *pattern, int pi, char c) {
  int negate;
  int matched;

  negate = 0;
  pi++;
  if (pattern[pi] == '!') {
    negate = 1;
    pi++;
  }
  matched = 0;
  while (pattern[pi] && pattern[pi] != ']') {
    if (pattern[pi + 1] == '-' && pattern[pi + 2] && pattern[pi + 2] != ']') {
      if (c >= pattern[pi] && c <= pattern[pi + 2])
        matched = 1;
      pi += 3;
    } else {
      if (c == pattern[pi])
        matched = 1;
      pi++;
    }
  }
  if (negate)
    matched = !matched;
  return (matched);
}

static int glob_match(const char *pattern, const char *str) {
  int pi;
  int si;

  pi = 0;
  si = 0;
  if (pattern[pi] == '*' && !pattern[pi + 1])
    return (str[0] != '.');
  while (pattern[pi] && str[si]) {
    if (pattern[pi] == '*') {
      pi++;
      if (!pattern[pi])
        return (1);
      while (str[si]) {
        if (glob_match(pattern + pi, str + si))
          return (1);
        si++;
      }
      return (glob_match(pattern + pi, str + si));
    }
    if (pattern[pi] == '?') {
      if (str[si] == '.')
        return (0);
    } else if (pattern[pi] == '[') {
      if (!match_bracket(pattern, pi, str[si]))
        return (0);
      while (pattern[pi] && pattern[pi] != ']')
        pi++;
    } else if (pattern[pi] != str[si])
      return (0);
    pi++;
    si++;
  }
  while (pattern[pi] == '*')
    pi++;
  return (!pattern[pi] && !str[si]);
}

static char **collect_matches(const char *pattern) {
  DIR *dir;
  struct dirent *entry;
  char **matches;
  int count;
  int cap;

  dir = opendir(".");
  if (!dir)
    return (NULL);
  matches = ft_calloc(16, sizeof(char *));
  count = 0;
  cap = 15;
  entry = readdir(dir);
  while (entry) {
    if (entry->d_name[0] != '.' && glob_match(pattern, entry->d_name)) {
      if (count >= cap) {
        cap *= 2;
        matches = ft_realloc_arr(matches, count, cap + 1);
      }
      matches[count++] = ft_strdup(entry->d_name);
    }
    entry = readdir(dir);
  }
  closedir(dir);
  if (count == 0) {
    free(matches);
    return (NULL);
  }
  matches[count] = NULL;
  return (matches);
}

static void sort_strings(char **arr) {
  int i;
  int j;
  char *tmp;

  if (!arr)
    return;
  i = 0;
  while (arr[i]) {
    j = i + 1;
    while (arr[j]) {
      if (ft_strncmp(arr[i], arr[j], ft_strlen(arr[i]) + 1) > 0) {
        tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
      }
      j++;
    }
    i++;
  }
}

void expand_glob_argv(t_node *node) {
  char **matches;
  char **new_argv;
  int i;
  int j;
  int k;

  if (!node || !node->argv)
    return;
  new_argv = ft_calloc(1, sizeof(char *));
  k = 0;
  i = 0;
  while (node->argv[i]) {
    if (has_glob_char(node->argv[i])) {
      matches = collect_matches(node->argv[i]);
      if (matches) {
        sort_strings(matches);
        j = 0;
        while (matches[j]) {
          new_argv = ft_realloc_arr(new_argv, k, k + 2);
          new_argv[k++] = matches[j++];
        }
        free(matches);
      } else {
        new_argv = ft_realloc_arr(new_argv, k, k + 2);
        new_argv[k++] = ft_strdup(node->argv[i]);
      }
    } else {
      new_argv = ft_realloc_arr(new_argv, k, k + 2);
      new_argv[k++] = ft_strdup(node->argv[i]);
    }
    i++;
  }
  new_argv[k] = NULL;
  free_str_arr(node->argv);
  node->argv = new_argv;
  node->argc = k;
}
