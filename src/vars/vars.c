/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vars.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

t_var *var_find(t_var *vars, const char *name) {
  while (vars) {
    if (ft_strncmp(vars->name, name, ft_strlen(name) + 1) == 0)
      return (vars);
    vars = vars->next;
  }
  return (NULL);
}

void var_set(t_shell *shell, const char *name, const char *value) {
  t_var *var;
  t_var *new_var;

  var = var_find(shell->vars, name);
  if (var) {
    free(var->value);
    var->value = ft_strdup(value);
    return;
  }
  new_var = ft_calloc(1, sizeof(t_var));
  if (!new_var)
    return;
  new_var->name = ft_strdup(name);
  new_var->value = ft_strdup(value);
  new_var->next = shell->vars;
  shell->vars = new_var;
}

char *var_get(t_shell *shell, const char *name) {
  t_var *var;

  var = var_find(shell->vars, name);
  if (var)
    return (var->value);
  return (env_get(shell->env, name));
}

void var_unset(t_shell *shell, const char *name) {
  t_var *cur;
  t_var *prev;

  prev = NULL;
  cur = shell->vars;
  while (cur) {
    if (ft_strncmp(cur->name, name, ft_strlen(name) + 1) == 0) {
      if (prev)
        prev->next = cur->next;
      else
        shell->vars = cur->next;
      free(cur->name);
      free(cur->value);
      free(cur);
      return;
    }
    prev = cur;
    cur = cur->next;
  }
}

void var_list_free(t_var *vars) {
  t_var *tmp;

  while (vars) {
    tmp = vars->next;
    free(vars->name);
    free(vars->value);
    free(vars);
    vars = tmp;
  }
}
