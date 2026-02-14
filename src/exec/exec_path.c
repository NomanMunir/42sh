/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <sys/stat.h>

static char *try_path(const char *dir, const char *cmd) {
  char *tmp;
  char *full;

  tmp = ft_strjoin(dir, "/");
  if (!tmp)
    return (NULL);
  full = ft_strjoin(tmp, cmd);
  free(tmp);
  return (full);
}

char *resolve_path(const char *cmd, char **env) {
  char *path_env;
  char **dirs;
  char *full;
  int i;
  struct stat sb;

  if (!cmd || !*cmd)
    return (NULL);
  if (ft_strchr(cmd, '/')) {
    if (stat(cmd, &sb) == 0 && (sb.st_mode & S_IXUSR))
      return (ft_strdup(cmd));
    return (NULL);
  }
  path_env = env_get(env, "PATH");
  if (!path_env)
    return (NULL);
  dirs = ft_split(path_env, ':');
  if (!dirs)
    return (NULL);
  i = -1;
  while (dirs[++i]) {
    full = try_path(dirs[i], cmd);
    if (full && stat(full, &sb) == 0 && (sb.st_mode & S_IXUSR)) {
      free_str_arr(dirs);
      return (full);
    }
    free(full);
  }
  free_str_arr(dirs);
  return (NULL);
}
