/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "shell.h"


/*
** Grammar (recursive descent):
**   list      : pipeline (('&&' | '||' | ';' | '&') pipeline)*
**   pipeline  : command ('|' command)*
**   command   : (assignment)* WORD+ (redirection)*
*/

static t_node *parse_command(t_token **cur);
static t_node *parse_pipeline(t_token **cur);
static t_node *parse_list(t_token **cur);

static int parse_redir(t_token **cur, t_node *cmd) {
  t_token_type rtype;
  int fd;

  rtype = (*cur)->type;
  if (rtype == TOK_REDIR_IN || rtype == TOK_HEREDOC || rtype == TOK_REDIR_IN_FD)
    fd = STDIN_FILENO;
  else
    fd = STDOUT_FILENO;
  *cur = (*cur)->next;
  if (!*cur || (*cur)->type != TOK_WORD)
    return (-1);
  redir_add_back(&cmd->redirs, redir_new(rtype, (*cur)->value, fd));
  *cur = (*cur)->next;
  return (0);
}

static void cmd_argv_add(t_node *cmd, const char *word) {
  char **new_argv;
  int i;

  new_argv = ft_calloc(cmd->argc + 2, sizeof(char *));
  if (!new_argv)
    return;
  i = 0;
  while (i < cmd->argc) {
    new_argv[i] = cmd->argv[i];
    i++;
  }
  new_argv[i] = ft_strdup(word);
  free(cmd->argv);
  cmd->argv = new_argv;
  cmd->argc++;
}

static t_node *parse_command(t_token **cur) {
  t_node *cmd;

  cmd = node_new(NODE_CMD);
  if (!cmd)
    return (NULL);
  while (*cur && (*cur)->type != TOK_EOF) {
    if (is_redir_token((*cur)->type)) {
      if (parse_redir(cur, cmd) < 0) {
        ft_putendl_fd("42sh: syntax error near redirection", 2);
        node_free(cmd);
        return (NULL);
      }
    } else if ((*cur)->type == TOK_WORD) {
      cmd_argv_add(cmd, (*cur)->value);
      *cur = (*cur)->next;
    } else
      break;
  }
  if (cmd->argc == 0 && !cmd->redirs) {
    node_free(cmd);
    return (NULL);
  }
  return (cmd);
}

static t_node *parse_pipeline(t_token **cur) {
  t_node *left;
  t_node *right;
  t_node *pipe_node;

  left = parse_command(cur);
  if (!left)
    return (NULL);
  while (*cur && (*cur)->type == TOK_PIPE) {
    *cur = (*cur)->next;
    right = parse_command(cur);
    if (!right) {
      ft_putendl_fd("42sh: syntax error near '|'", 2);
      node_free(left);
      return (NULL);
    }
    pipe_node = node_new(NODE_PIPE);
    pipe_node->left = left;
    pipe_node->right = right;
    left = pipe_node;
  }
  return (left);
}

static t_node *make_binary(t_node_type type, t_node *left, t_node *right) {
  t_node *node;

  node = node_new(type);
  if (!node)
    return (NULL);
  node->left = left;
  node->right = right;
  return (node);
}

static t_node *parse_list(t_token **cur) {
  t_node *left;
  t_node *right;
  t_node_type ntype;

  left = parse_pipeline(cur);
  if (!left)
    return (NULL);
  while (*cur && ((*cur)->type == TOK_AND || (*cur)->type == TOK_OR ||
                  (*cur)->type == TOK_SEMI || (*cur)->type == TOK_AMP)) {
    if ((*cur)->type == TOK_AND)
      ntype = NODE_AND;
    else if ((*cur)->type == TOK_OR)
      ntype = NODE_OR;
    else if ((*cur)->type == TOK_AMP)
      ntype = NODE_BG;
    else
      ntype = NODE_SEQ;
    *cur = (*cur)->next;
    if (!*cur || (*cur)->type == TOK_EOF) {
      if (ntype == NODE_SEQ || ntype == NODE_BG)
        return (make_binary(ntype, left, NULL));
      ft_putendl_fd("42sh: syntax error near operator", 2);
      return (node_free(left), NULL);
    }
    right = parse_pipeline(cur);
    if (!right && (ntype == NODE_AND || ntype == NODE_OR)) {
      ft_putendl_fd("42sh: syntax error near operator", 2);
      return (node_free(left), NULL);
    }
    left = make_binary(ntype, left, right);
  }
  return (left);
}

t_node *parse(t_token *tokens) {
  t_token *cur;

  cur = tokens;
  if (!cur || cur->type == TOK_EOF)
    return (NULL);
  return (parse_list(&cur));
}
