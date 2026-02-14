/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_H
#define SHELL_H

/* ---- System headers ---- */
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


/* ---- Project headers ---- */
#include "ast.h"
#include "libft.h"
#include "token.h"


/* ---- Constants ---- */
#define SHELL_NAME "42sh"
#define PROMPT "42sh$ "

/* ---- Internal variable ---- */
typedef struct s_var {
  char *name;
  char *value;
  struct s_var *next;
} t_var;

/* ---- Alias ---- */
typedef struct s_alias {
  char *name;
  char *value;
  struct s_alias *next;
} t_alias;

/* ---- Job status ---- */
typedef enum e_job_status { JOB_RUNNING, JOB_STOPPED, JOB_DONE } t_job_status;

/* ---- Job ---- */
typedef struct s_job {
  int id;
  pid_t pid;
  char *cmd;
  t_job_status status;
  struct s_job *next;
} t_job;

/* ---- Shell state ---- */
typedef struct s_shell {
  char **env;
  t_var *vars;
  t_alias *aliases;
  t_job *jobs;
  int job_count;
  int exit_code;
  int running;
} t_shell;

/* ---- init.c ---- */
t_shell *shell_init(char **envp);
void shell_destroy(t_shell *shell);

/* ---- env.c ---- */
char **env_copy(char **envp);
void env_free(char **env);
char *env_get(char **env, const char *name);
void env_set(t_shell *shell, const char *name, const char *value);
void env_unset(t_shell *shell, const char *name);
char **env_realloc_add(char **env, const char *entry);
void free_str_arr(char **arr);

/* ---- signals.c ---- */
void setup_signals(void);

/* ---- utils.c ---- */
char **ft_realloc_arr(char **arr, int old_count, int new_cap);
void process_input(const char *line, t_shell *shell);

/* ---- vars/vars.c ---- */
t_var *var_find(t_var *vars, const char *name);
void var_set(t_shell *shell, const char *name, const char *value);
char *var_get(t_shell *shell, const char *name);
void var_unset(t_shell *shell, const char *name);
void var_list_free(t_var *vars);

/* ---- expand/expand.c ---- */
char *expand_variables(const char *word, t_shell *shell);
void expand_argv(t_node *node, t_shell *shell);

/* ---- expand/expand_tilde.c ---- */
char *expand_tilde(const char *word, t_shell *shell);
void expand_tilde_argv(t_node *node, t_shell *shell);

/* ---- expand/expand_cmd.c ---- */
char *expand_command_sub(const char *cmd, t_shell *shell);
char *expand_cmd_substitutions(const char *word, t_shell *shell);

/* ---- expand/expand_glob.c ---- */
void expand_glob_argv(t_node *node);

/* ---- exec/executor.c ---- */
int execute_ast(t_node *node, t_shell *shell);

/* ---- exec/exec_cmd.c ---- */
int execute_simple_cmd(t_node *node, t_shell *shell);

/* ---- exec/exec_pipe.c ---- */
int execute_pipe(t_node *node, t_shell *shell);

/* ---- exec/exec_redir.c ---- */
int apply_redirections(t_redir *redirs);

/* ---- exec/exec_path.c ---- */
char *resolve_path(const char *cmd, char **env);

/* ---- builtins/builtins.c ---- */
int run_builtin(char **argv, t_shell *shell);

/* ---- Individual builtins ---- */
int builtin_cd(char **argv, t_shell *shell);
int builtin_echo(char **argv, t_shell *shell);
int builtin_exit(char **argv, t_shell *shell);
int builtin_type(char **argv, t_shell *shell);
int builtin_export(char **argv, t_shell *shell);
int builtin_unset(char **argv, t_shell *shell);
int builtin_set(char **argv, t_shell *shell);
int builtin_jobs(char **argv, t_shell *shell);
int builtin_fg(char **argv, t_shell *shell);
int builtin_bg(char **argv, t_shell *shell);
int builtin_test(char **argv, t_shell *shell);
int builtin_alias(char **argv, t_shell *shell);
int builtin_unalias(char **argv, t_shell *shell);
int builtin_fc(char **argv, t_shell *shell);

/* ---- builtins/alias.c ---- */
void alias_set(t_shell *shell, const char *name, const char *value);
void alias_remove(t_shell *shell, const char *name);
char *alias_expand(const char *input, t_shell *shell);
void alias_list_free(t_alias *aliases);

/* ---- builtins/history.c ---- */
char *expand_history(const char *input);

/* ---- jobs/jobs.c ---- */
void job_add(t_shell *shell, pid_t pid, const char *cmd);
void job_remove(t_shell *shell, int id);
void job_update_status(t_shell *shell);
void job_list_free(t_job *jobs);

#endif
