/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_jobs.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static const char *job_status_str(t_job_status status) {
  if (status == JOB_RUNNING)
    return ("Running");
  if (status == JOB_STOPPED)
    return ("Stopped");
  if (status == JOB_DONE)
    return ("Done");
  return ("Unknown");
}

int builtin_jobs(char **argv, t_shell *shell) {
  t_job *cur;

  (void)argv;
  job_update_status(shell);
  cur = shell->jobs;
  while (cur) {
    ft_putstr_fd("[", 1);
    ft_putnbr_fd(cur->id, 1);
    ft_putstr_fd("]  ", 1);
    ft_putstr_fd((char *)job_status_str(cur->status), 1);
    ft_putstr_fd("\t\t", 1);
    ft_putendl_fd(cur->cmd, 1);
    cur = cur->next;
  }
  return (0);
}
