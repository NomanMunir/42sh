/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int env_count(char **envp) {
  int i;

  i = 0;
  while (envp && envp[i])
    i++;
  return (i);
}

char **env_copy(char **envp) {
  char **env;
  int count;
  int i;

  count = env_count(envp);
  env = ft_calloc(count + 1, sizeof(char *));
  if (!env)
    return (NULL);
  i = 0;
  while (i < count) {
    env[i] = ft_strdup(envp[i]);
    if (!env[i]) {
      env_free(env);
      return (NULL);
    }
    i++;
  }
  return (env);
}

void env_free(char **env) {
  int i;

  if (!env)
    return;
  i = 0;
  while (env[i]) {
    free(env[i]);
    i++;
  }
  free(env);
}

char *env_get(char **env, const char *name) {
  int i;
  size_t nlen;

  if (!env || !name)
    return (NULL);
  nlen = ft_strlen(name);
  i = 0;
  while (env[i]) {
    if (ft_strncmp(env[i], name, nlen) == 0 && env[i][nlen] == '=')
      return (env[i] + nlen + 1);
    i++;
  }
  return (NULL);
}

void env_set(t_shell *shell, const char *name, const char *value) {
  int i;
  size_t nlen;
  char *entry;
  char *tmp;

  nlen = ft_strlen(name);
  tmp = ft_strjoin(name, "=");
  entry = ft_strjoin(tmp, value);
  free(tmp);
  i = 0;
  while (shell->env[i]) {
    if (ft_strncmp(shell->env[i], name, nlen) == 0 &&
        shell->env[i][nlen] == '=') {
      free(shell->env[i]);
      shell->env[i] = entry;
      return;
    }
    i++;
  }
  shell->env = env_realloc_add(shell->env, entry);
  free(entry);
}

void env_unset(t_shell *shell, const char *name) {
  int i;
  int j;
  size_t nlen;

  if (!shell->env || !name)
    return;
  nlen = ft_strlen(name);
  i = 0;
  while (shell->env[i]) {
    if (ft_strncmp(shell->env[i], name, nlen) == 0 &&
        shell->env[i][nlen] == '=') {
      free(shell->env[i]);
      j = i;
      while (shell->env[j + 1]) {
        shell->env[j] = shell->env[j + 1];
        j++;
      }
      shell->env[j] = NULL;
      return;
    }
    i++;
  }
}

char **env_realloc_add(char **env, const char *entry) {
  char **new_env;
  int count;
  int i;

  count = env_count(env);
  new_env = ft_calloc(count + 2, sizeof(char *));
  if (!new_env)
    return (env);
  i = 0;
  while (i < count) {
    new_env[i] = env[i];
    i++;
  }
  new_env[i] = ft_strdup(entry);
  new_env[i + 1] = NULL;
  free(env);
  return (new_env);
}

void free_str_arr(char **arr) {
  int i;

  if (!arr)
    return;
  i = 0;
  while (arr[i])
    free(arr[i++]);
  free(arr);
}
