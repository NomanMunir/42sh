/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void job_add(t_shell *shell, pid_t pid, const char *cmd) {
  t_job *job;
  t_job *cur;

  job = ft_calloc(1, sizeof(t_job));
  if (!job)
    return;
  shell->job_count++;
  job->id = shell->job_count;
  job->pid = pid;
  job->cmd = ft_strdup(cmd);
  job->status = JOB_RUNNING;
  job->next = NULL;
  if (!shell->jobs) {
    shell->jobs = job;
    return;
  }
  cur = shell->jobs;
  while (cur->next)
    cur = cur->next;
  cur->next = job;
}

void job_remove(t_shell *shell, int id) {
  t_job *cur;
  t_job *prev;

  prev = NULL;
  cur = shell->jobs;
  while (cur) {
    if (cur->id == id) {
      if (prev)
        prev->next = cur->next;
      else
        shell->jobs = cur->next;
      free(cur->cmd);
      free(cur);
      return;
    }
    prev = cur;
    cur = cur->next;
  }
}

void job_update_status(t_shell *shell) {
  t_job *cur;
  int status;
  pid_t result;

  cur = shell->jobs;
  while (cur) {
    if (cur->status == JOB_RUNNING) {
      result = waitpid(cur->pid, &status, WNOHANG);
      if (result > 0) {
        cur->status = JOB_DONE;
        ft_putstr_fd("[", 1);
        ft_putnbr_fd(cur->id, 1);
        ft_putstr_fd("]  Done\t\t", 1);
        ft_putendl_fd(cur->cmd, 1);
      }
    }
    cur = cur->next;
  }
}

void job_list_free(t_job *jobs) {
  t_job *tmp;

  while (jobs) {
    tmp = jobs->next;
    free(jobs->cmd);
    free(jobs);
    jobs = tmp;
  }
}
