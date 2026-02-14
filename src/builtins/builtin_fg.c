/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_fg.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static t_job *find_job(t_shell *shell, int id) {
  t_job *cur;

  cur = shell->jobs;
  while (cur) {
    if (cur->id == id)
      return (cur);
    cur = cur->next;
  }
  return (NULL);
}

static t_job *last_job(t_shell *shell) {
  t_job *cur;
  t_job *last;

  last = NULL;
  cur = shell->jobs;
  while (cur) {
    last = cur;
    cur = cur->next;
  }
  return (last);
}

int builtin_fg(char **argv, t_shell *shell) {
  t_job *job;
  int status;
  int id;

  if (argv[1])
    id = ft_atoi(argv[1]);
  else {
    job = last_job(shell);
    if (!job) {
      ft_putendl_fd("42sh: fg: no current job", 2);
      return (1);
    }
    id = job->id;
  }
  job = find_job(shell, id);
  if (!job) {
    ft_putendl_fd("42sh: fg: no such job", 2);
    return (1);
  }
  ft_putendl_fd(job->cmd, 1);
  if (job->status == JOB_STOPPED)
    kill(job->pid, SIGCONT);
  waitpid(job->pid, &status, 0);
  job_remove(shell, id);
  if (WIFEXITED(status))
    return (WEXITSTATUS(status));
  return (1);
}
