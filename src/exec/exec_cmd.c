/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "shell.h"

static int exec_external(t_node *node, t_shell *shell) {
  pid_t pid;
  int status;
  char *path;

  path = resolve_path(node->argv[0], shell->env);
  if (!path) {
    ft_putstr_fd("42sh: ", 2);
    ft_putstr_fd(node->argv[0], 2);
    ft_putendl_fd(": command not found", 2);
    return (127);
  }
  pid = fork();
  if (pid < 0)
    return (free(path), 1);
  if (pid == 0) {
    if (apply_redirections(node->redirs) < 0)
      exit(1);
    execve(path, node->argv, shell->env);
    ft_putstr_fd("42sh: ", 2);
    perror(node->argv[0]);
    exit(126);
  }
  free(path);
  waitpid(pid, &status, 0);
  if (WIFEXITED(status))
    return (WEXITSTATUS(status));
  if (WIFSIGNALED(status))
    return (128 + WTERMSIG(status));
  return (1);
}

static char *expand_value(const char *raw, t_shell *shell) {
  char *after_cmd;
  char *after_var;

  after_cmd = expand_cmd_substitutions(raw, shell);
  after_var = expand_variables(after_cmd, shell);
  free(after_cmd);
  return (after_var);
}

static int handle_assignment(t_node *node, t_shell *shell) {
  char *eq;
  char *name;
  char *value;
  int i;

  if (!node->argv || !node->argv[0])
    return (0);
  i = 0;
  while (node->argv[i]) {
    eq = ft_strchr(node->argv[i], '=');
    if (!eq || eq == node->argv[i])
      return (-1);
    name = ft_substr(node->argv[i], 0, eq - node->argv[i]);
    value = expand_value(eq + 1, shell);
    var_set(shell, name, value);
    free(name);
    free(value);
    i++;
  }
  return (0);
}

static int is_assignment(const char *s) {
  int i;

  if (!s || !ft_isalpha(s[0]))
    return (0);
  i = 1;
  while (s[i] && s[i] != '=') {
    if (!ft_isalnum(s[i]) && s[i] != '_')
      return (0);
    i++;
  }
  return (s[i] == '=');
}

static void run_expansions(t_node *node, t_shell *shell) {
  int i;
  char *tmp;

  expand_tilde_argv(node, shell);
  i = 0;
  while (node->argv[i]) {
    tmp = expand_cmd_substitutions(node->argv[i], shell);
    free(node->argv[i]);
    node->argv[i] = tmp;
    i++;
  }
  expand_argv(node, shell);
  expand_glob_argv(node);
}

static int save_fds(int saved[3]) {
  saved[0] = dup(STDIN_FILENO);
  saved[1] = dup(STDOUT_FILENO);
  saved[2] = dup(STDERR_FILENO);
  if (saved[0] < 0 || saved[1] < 0 || saved[2] < 0)
    return (-1);
  return (0);
}

static void restore_fds(int saved[3]) {
  dup2(saved[0], STDIN_FILENO);
  dup2(saved[1], STDOUT_FILENO);
  dup2(saved[2], STDERR_FILENO);
  close(saved[0]);
  close(saved[1]);
  close(saved[2]);
}

static int is_builtin_cmd(const char *name) {
  static const char *builtins[] = {
      "cd", "echo", "exit", "type", "export", "unset",   "set", "jobs",
      "fg", "bg",   "test", "[",    "alias",  "unalias", "fc",  NULL};
  int i;

  i = 0;
  while (builtins[i]) {
    if (ft_strncmp(name, builtins[i], ft_strlen(builtins[i]) + 1) == 0)
      return (1);
    i++;
  }
  return (0);
}

static int run_builtin_with_redir(t_node *node, t_shell *shell) {
  int ret;
  int saved_fds[3];

  if (!node->redirs)
    return (run_builtin(node->argv, shell));
  if (save_fds(saved_fds) < 0)
    return (1);
  if (apply_redirections(node->redirs) < 0) {
    restore_fds(saved_fds);
    return (1);
  }
  ret = run_builtin(node->argv, shell);
  restore_fds(saved_fds);
  return (ret);
}

int execute_simple_cmd(t_node *node, t_shell *shell) {
  if (!node->argv || !node->argv[0]) {
    if (node->redirs)
      return (apply_redirections(node->redirs) < 0);
    return (0);
  }
  if (is_assignment(node->argv[0]) && !node->argv[1])
    return (handle_assignment(node, shell));
  run_expansions(node, shell);
  if (!node->argv[0] || !node->argv[0][0])
    return (0);
  if (is_builtin_cmd(node->argv[0]))
    return (run_builtin_with_redir(node, shell));
  return (exec_external(node, shell));
}
