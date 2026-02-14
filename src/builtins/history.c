/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static char *expand_bang_n(const char *input, int *i) {
  int num;
  HIST_ENTRY *entry;

  (*i)++;
  num = 0;
  while (input[*i] && ft_isdigit(input[*i]))
    num = num * 10 + (input[(*i)++] - '0');
  entry = history_get(num);
  if (entry)
    return (ft_strdup(entry->line));
  ft_putstr_fd("42sh: !", 2);
  ft_putnbr_fd(num, 2);
  ft_putendl_fd(": event not found", 2);
  return (NULL);
}

static char *expand_bang_word(const char *input, int *i) {
  int start;
  char *word;
  HIST_ENTRY **list;
  int j;

  start = *i;
  while (input[*i] && input[*i] != ' ' && input[*i] != '\t')
    (*i)++;
  word = ft_substr(input, start, *i - start);
  list = history_list();
  if (!list) {
    free(word);
    return (NULL);
  }
  j = 0;
  while (list[j])
    j++;
  while (--j >= 0) {
    if (ft_strncmp(list[j]->line, word, ft_strlen(word)) == 0) {
      free(word);
      return (ft_strdup(list[j]->line));
    }
  }
  ft_putstr_fd("42sh: !", 2);
  ft_putstr_fd(word, 2);
  ft_putendl_fd(": event not found", 2);
  free(word);
  return (NULL);
}

static char *get_last_history(void) {
  HIST_ENTRY **list;
  int j;

  list = history_list();
  if (!list)
    return (NULL);
  j = 0;
  while (list[j])
    j++;
  if (j > 0)
    return (ft_strdup(list[j - 1]->line));
  return (NULL);
}

char *expand_history(const char *input) {
  char *result;
  char *expanded;
  char *tmp;
  int i;

  result = ft_strdup("");
  i = 0;
  while (input[i]) {
    if (input[i] == '!' && input[i + 1] == '!') {
      expanded = get_last_history();
      i += 2;
    } else if (input[i] == '!' && ft_isdigit(input[i + 1])) {
      i++;
      expanded = expand_bang_n(input, &i);
    } else if (input[i] == '!' && ft_isalpha(input[i + 1])) {
      i++;
      expanded = expand_bang_word(input, &i);
    } else {
      expanded = ft_substr(input, i, 1);
      i++;
    }
    if (!expanded) {
      free(result);
      return (NULL);
    }
    tmp = ft_strjoin(result, expanded);
    free(result);
    free(expanded);
    result = tmp;
  }
  return (result);
}

int builtin_fc(char **argv, t_shell *shell) {
  HIST_ENTRY **list;
  int i;
  int start;
  int end;
  int count;

  (void)shell;
  list = history_list();
  if (!list)
    return (0);
  count = 0;
  while (list[count])
    count++;
  if (argv[1] && ft_strncmp(argv[1], "-l", 3) == 0) {
    start = count - 16;
    if (start < 0)
      start = 0;
    end = count;
    if (argv[2])
      start = ft_atoi(argv[2]) - 1;
    if (argv[2] && argv[3])
      end = ft_atoi(argv[3]);
    if (start < 0)
      start = 0;
    if (end > count)
      end = count;
    i = start;
    while (i < end) {
      ft_putnbr_fd(i + 1, 1);
      ft_putchar_fd('\t', 1);
      ft_putendl_fd(list[i]->line, 1);
      i++;
    }
    return (0);
  }
  if (count > 0) {
    ft_putendl_fd(list[count - 1]->line, 1);
    process_input(list[count - 1]->line, shell);
  }
  return (shell->exit_code);
}
