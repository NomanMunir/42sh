/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "shell.h"
#include <fcntl.h>


static int open_redir_file(t_redir *redir) {
  int fd;

  fd = -1;
  if (redir->type == TOK_REDIR_OUT)
    fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  else if (redir->type == TOK_REDIR_APPEND)
    fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
  else if (redir->type == TOK_REDIR_IN)
    fd = open(redir->target, O_RDONLY);
  if (fd < 0) {
    ft_putstr_fd("42sh: ", 2);
    ft_putstr_fd(redir->target, 2);
    ft_putendl_fd(": No such file or directory", 2);
  }
  return (fd);
}

static int apply_heredoc(t_redir *redir) {
  int pipefd[2];
  char *line;

  if (pipe(pipefd) < 0)
    return (-1);
  while (1) {
    line = readline("> ");
    if (!line ||
        ft_strncmp(line, redir->target, ft_strlen(redir->target) + 1) == 0) {
      free(line);
      break;
    }
    ft_putendl_fd(line, pipefd[1]);
    free(line);
  }
  close(pipefd[1]);
  if (dup2(pipefd[0], STDIN_FILENO) < 0) {
    close(pipefd[0]);
    return (-1);
  }
  close(pipefd[0]);
  return (0);
}

static int apply_fd_redir(t_redir *redir) {
  int target_fd;
  int src_fd;

  if (ft_strncmp(redir->target, "-", 2) == 0) {
    close(redir->fd);
    return (0);
  }
  target_fd = ft_atoi(redir->target);
  if (redir->type == TOK_REDIR_OUT_FD)
    src_fd = STDOUT_FILENO;
  else
    src_fd = STDIN_FILENO;
  (void)src_fd;
  if (dup2(target_fd, redir->fd) < 0) {
    ft_putendl_fd("42sh: bad file descriptor", 2);
    return (-1);
  }
  return (0);
}

int apply_redirections(t_redir *redirs) {
  int fd;

  while (redirs) {
    if (redirs->type == TOK_HEREDOC) {
      if (apply_heredoc(redirs) < 0)
        return (-1);
    } else if (redirs->type == TOK_REDIR_OUT_FD ||
               redirs->type == TOK_REDIR_IN_FD) {
      if (apply_fd_redir(redirs) < 0)
        return (-1);
    } else {
      fd = open_redir_file(redirs);
      if (fd < 0)
        return (-1);
      if (dup2(fd, redirs->fd) < 0) {
        close(fd);
        return (-1);
      }
      close(fd);
    }
    redirs = redirs->next;
  }
  return (0);
}
