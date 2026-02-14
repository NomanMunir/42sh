/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "shell.h"


t_node *node_new(t_node_type type) {
  t_node *node;

  node = ft_calloc(1, sizeof(t_node));
  if (!node)
    return (NULL);
  node->type = type;
  return (node);
}

static void redir_free(t_redir *redir) {
  t_redir *tmp;

  while (redir) {
    tmp = redir->next;
    free(redir->target);
    free(redir);
    redir = tmp;
  }
}

static void free_str_array(char **arr) {
  int i;

  if (!arr)
    return;
  i = 0;
  while (arr[i])
    free(arr[i++]);
  free(arr);
}

void node_free(t_node *node) {
  if (!node)
    return;
  free_str_array(node->argv);
  free_str_array(node->assigns);
  redir_free(node->redirs);
  node_free(node->left);
  node_free(node->right);
  free(node);
}

t_redir *redir_new(t_token_type type, const char *target, int fd) {
  t_redir *redir;

  redir = ft_calloc(1, sizeof(t_redir));
  if (!redir)
    return (NULL);
  redir->type = type;
  if (target)
    redir->target = ft_strdup(target);
  redir->fd = fd;
  redir->next = NULL;
  return (redir);
}

void redir_add_back(t_redir **head, t_redir *new_redir) {
  t_redir *cur;

  if (!head || !new_redir)
    return;
  if (!*head) {
    *head = new_redir;
    return;
  }
  cur = *head;
  while (cur->next)
    cur = cur->next;
  cur->next = new_redir;
}

int is_redir_token(t_token_type type) {
  return (type == TOK_REDIR_IN || type == TOK_REDIR_OUT ||
          type == TOK_REDIR_APPEND || type == TOK_HEREDOC ||
          type == TOK_REDIR_OUT_FD || type == TOK_REDIR_IN_FD);
}
