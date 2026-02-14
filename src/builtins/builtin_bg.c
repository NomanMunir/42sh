/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_bg.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

int builtin_bg(char **argv, t_shell *shell) {
  t_job *cur;
  int id;

  if (argv[1])
    id = ft_atoi(argv[1]);
  else {
    cur = shell->jobs;
    if (!cur) {
      ft_putendl_fd("42sh: bg: no current job", 2);
      return (1);
    }
    while (cur->next)
      cur = cur->next;
    id = cur->id;
  }
  cur = shell->jobs;
  while (cur) {
    if (cur->id == id) {
      if (cur->status == JOB_STOPPED) {
        kill(cur->pid, SIGCONT);
        cur->status = JOB_RUNNING;
      }
      ft_putstr_fd("[", 1);
      ft_putnbr_fd(cur->id, 1);
      ft_putstr_fd("]  ", 1);
      ft_putendl_fd(cur->cmd, 1);
      return (0);
    }
    cur = cur->next;
  }
  ft_putendl_fd("42sh: bg: no such job", 2);
  return (1);
}
