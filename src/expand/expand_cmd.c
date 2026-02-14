/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static char *read_pipe_output(int fd) {
  char *result;
  char *tmp;
  char buf[4096];
  int n;

  result = ft_strdup("");
  while (1) {
    n = read(fd, buf, sizeof(buf) - 1);
    if (n <= 0)
      break;
    buf[n] = '\0';
    tmp = ft_strjoin(result, buf);
    free(result);
    result = tmp;
  }
  n = ft_strlen(result);
  while (n > 0 && result[n - 1] == '\n')
    result[--n] = '\0';
  return (result);
}

char *expand_command_sub(const char *cmd, t_shell *shell) {
  int pipefd[2];
  pid_t pid;
  char *output;
  int status;

  if (pipe(pipefd) < 0)
    return (ft_strdup(""));
  pid = fork();
  if (pid < 0) {
    close(pipefd[0]);
    close(pipefd[1]);
    return (ft_strdup(""));
  }
  if (pid == 0) {
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
    process_input(cmd, shell);
    exit(shell->exit_code);
  }
  close(pipefd[1]);
  output = read_pipe_output(pipefd[0]);
  close(pipefd[0]);
  waitpid(pid, &status, 0);
  return (output);
}

static int find_closing_paren(const char *s, int start) {
  int depth;
  int i;

  depth = 1;
  i = start;
  while (s[i] && depth > 0) {
    if (s[i] == '(')
      depth++;
    else if (s[i] == ')')
      depth--;
    if (depth > 0)
      i++;
  }
  if (depth == 0)
    return (i);
  return (-1);
}

static char *append_str(char *s1, const char *s2) {
  char *result;

  result = ft_strjoin(s1, s2);
  free(s1);
  return (result);
}

static char *append_chr(char *s, char c) {
  char buf[2];

  buf[0] = c;
  buf[1] = '\0';
  return (append_str(s, buf));
}

char *expand_cmd_substitutions(const char *word, t_shell *shell) {
  char *result;
  char *subcmd;
  char *output;
  int i;
  int end;

  result = ft_strdup("");
  i = 0;
  while (word[i]) {
    if (word[i] == '$' && word[i + 1] == '(') {
      end = find_closing_paren(word, i + 2);
      if (end < 0) {
        result = append_chr(result, word[i++]);
        continue;
      }
      subcmd = ft_substr(word, i + 2, end - (i + 2));
      output = expand_command_sub(subcmd, shell);
      result = append_str(result, output);
      free(subcmd);
      free(output);
      i = end + 1;
    } else
      result = append_chr(result, word[i++]);
  }
  return (result);
}
