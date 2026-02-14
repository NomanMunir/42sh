/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "shell.h"


static int execute_bg(t_node *node, t_shell *shell) {
  pid_t pid;

  if (!node->left)
    return (0);
  pid = fork();
  if (pid < 0)
    return (1);
  if (pid == 0)
    exit(execute_ast(node->left, shell));
  job_add(shell, pid, node->left->argv ? node->left->argv[0] : "bg");
  ft_putstr_fd("[", 1);
  ft_putnbr_fd(shell->job_count, 1);
  ft_putstr_fd("] ", 1);
  ft_putnbr_fd(pid, 1);
  ft_putstr_fd("\n", 1);
  return (0);
}

int execute_ast(t_node *node, t_shell *shell) {
  int ret;

  if (!node)
    return (0);
  if (node->type == NODE_CMD)
    return (execute_simple_cmd(node, shell));
  if (node->type == NODE_PIPE)
    return (execute_pipe(node, shell));
  if (node->type == NODE_SEQ) {
    execute_ast(node->left, shell);
    return (execute_ast(node->right, shell));
  }
  if (node->type == NODE_AND) {
    ret = execute_ast(node->left, shell);
    shell->exit_code = ret;
    if (ret == 0)
      return (execute_ast(node->right, shell));
    return (ret);
  }
  if (node->type == NODE_OR) {
    ret = execute_ast(node->left, shell);
    shell->exit_code = ret;
    if (ret != 0)
      return (execute_ast(node->right, shell));
    return (ret);
  }
  if (node->type == NODE_BG)
    return (execute_bg(node, shell));
  return (1);
}
