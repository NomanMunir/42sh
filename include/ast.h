/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
#define AST_H

#include "token.h"

typedef enum e_node_type {
  NODE_CMD,
  NODE_PIPE,
  NODE_AND,
  NODE_OR,
  NODE_SEQ,
  NODE_BG
} t_node_type;

typedef struct s_redir {
  t_token_type type;
  char *target;
  int fd;
  struct s_redir *next;
} t_redir;

typedef struct s_node {
  t_node_type type;
  char **argv;
  int argc;
  t_redir *redirs;
  char **assigns;
  struct s_node *left;
  struct s_node *right;
} t_node;

/* parser/parser.c */
t_node *parse(t_token *tokens);

/* parser/parser_utils.c */
t_node *node_new(t_node_type type);
void node_free(t_node *node);
t_redir *redir_new(t_token_type type, const char *target, int fd);
void redir_add_back(t_redir **head, t_redir *new_redir);
int is_redir_token(t_token_type type);

#endif
