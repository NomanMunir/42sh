/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "shell.h"


static void pipe_child_left(int *pipefd, t_node *left, t_shell *shell) {
  close(pipefd[0]);
  if (dup2(pipefd[1], STDOUT_FILENO) < 0)
    exit(EXIT_FAILURE);
  close(pipefd[1]);
  exit(execute_ast(left, shell));
}

static void pipe_child_right(int *pipefd, t_node *right, t_shell *shell) {
  close(pipefd[1]);
  if (dup2(pipefd[0], STDIN_FILENO) < 0)
    exit(EXIT_FAILURE);
  close(pipefd[0]);
  exit(execute_ast(right, shell));
}

int execute_pipe(t_node *node, t_shell *shell) {
  int pipefd[2];
  pid_t pid_left;
  pid_t pid_right;
  int status;

  if (pipe(pipefd) < 0)
    return (1);
  pid_left = fork();
  if (pid_left == 0)
    pipe_child_left(pipefd, node->left, shell);
  pid_right = fork();
  if (pid_right == 0)
    pipe_child_right(pipefd, node->right, shell);
  close(pipefd[0]);
  close(pipefd[1]);
  waitpid(pid_left, NULL, 0);
  waitpid(pid_right, &status, 0);
  if (WIFEXITED(status))
    return (WEXITSTATUS(status));
  return (1);
}
