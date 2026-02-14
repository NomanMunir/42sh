/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_test.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <sys/stat.h>

static int file_test(const char *flag, const char *path) {
  struct stat sb;

  if (stat(path, &sb) < 0)
    return (1);
  if (ft_strncmp(flag, "-e", 3) == 0)
    return (0);
  if (ft_strncmp(flag, "-f", 3) == 0)
    return (S_ISREG(sb.st_mode) ? 0 : 1);
  if (ft_strncmp(flag, "-d", 3) == 0)
    return (S_ISDIR(sb.st_mode) ? 0 : 1);
  if (ft_strncmp(flag, "-r", 3) == 0)
    return ((sb.st_mode & S_IRUSR) ? 0 : 1);
  if (ft_strncmp(flag, "-w", 3) == 0)
    return ((sb.st_mode & S_IWUSR) ? 0 : 1);
  if (ft_strncmp(flag, "-x", 3) == 0)
    return ((sb.st_mode & S_IXUSR) ? 0 : 1);
  if (ft_strncmp(flag, "-s", 3) == 0)
    return (sb.st_size > 0 ? 0 : 1);
  return (1);
}

static int string_test(char **argv, int i) {
  if (ft_strncmp(argv[i], "-z", 3) == 0 && argv[i + 1])
    return (argv[i + 1][0] == '\0' ? 0 : 1);
  if (ft_strncmp(argv[i], "-n", 3) == 0 && argv[i + 1])
    return (argv[i + 1][0] != '\0' ? 0 : 1);
  if (argv[i + 1] && ft_strncmp(argv[i + 1], "=", 2) == 0 && argv[i + 2])
    return (ft_strncmp(argv[i], argv[i + 2], ft_strlen(argv[i]) + 1) == 0 ? 0
                                                                          : 1);
  if (argv[i + 1] && ft_strncmp(argv[i + 1], "!=", 3) == 0 && argv[i + 2])
    return (ft_strncmp(argv[i], argv[i + 2], ft_strlen(argv[i]) + 1) != 0 ? 0
                                                                          : 1);
  return (2);
}

static int int_test(char **argv, int i) {
  int l;
  int r;

  if (!argv[i + 1] || !argv[i + 2])
    return (2);
  l = ft_atoi(argv[i]);
  r = ft_atoi(argv[i + 2]);
  if (ft_strncmp(argv[i + 1], "-eq", 4) == 0)
    return (l == r ? 0 : 1);
  if (ft_strncmp(argv[i + 1], "-ne", 4) == 0)
    return (l != r ? 0 : 1);
  if (ft_strncmp(argv[i + 1], "-lt", 4) == 0)
    return (l < r ? 0 : 1);
  if (ft_strncmp(argv[i + 1], "-gt", 4) == 0)
    return (l > r ? 0 : 1);
  if (ft_strncmp(argv[i + 1], "-le", 4) == 0)
    return (l <= r ? 0 : 1);
  if (ft_strncmp(argv[i + 1], "-ge", 4) == 0)
    return (l >= r ? 0 : 1);
  return (2);
}

static int eval_test(char **argv, int i) {
  int result;

  if (!argv[i])
    return (1);
  if (ft_strncmp(argv[i], "!", 2) == 0)
    return (eval_test(argv, i + 1) == 0 ? 1 : 0);
  if (argv[i][0] == '-' && argv[i][1] && !argv[i][2] && argv[i + 1]) {
    if (ft_strchr("efdRwxs", argv[i][1]))
      return (file_test(argv[i], argv[i + 1]));
    if (argv[i][1] == 'z' || argv[i][1] == 'n')
      return (string_test(argv, i));
  }
  result = string_test(argv, i);
  if (result != 2)
    return (result);
  result = int_test(argv, i);
  if (result != 2)
    return (result);
  return (argv[i][0] != '\0' ? 0 : 1);
}

int builtin_test(char **argv, t_shell *shell) {
  int argc;
  int is_bracket;

  (void)shell;
  is_bracket = (ft_strncmp(argv[0], "[", 2) == 0);
  argc = 0;
  while (argv[argc])
    argc++;
  if (is_bracket) {
    if (argc < 2 || ft_strncmp(argv[argc - 1], "]", 2) != 0) {
      ft_putendl_fd("42sh: [: missing ']'", 2);
      return (2);
    }
    argv[argc - 1] = NULL;
  }
  return (eval_test(argv, 1));
}
