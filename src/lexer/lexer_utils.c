/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include "token.h"

t_token *token_new(t_token_type type, const char *value) {
  t_token *tok;

  tok = ft_calloc(1, sizeof(t_token));
  if (!tok)
    return (NULL);
  tok->type = type;
  if (value)
    tok->value = ft_strdup(value);
  tok->next = NULL;
  return (tok);
}

void token_add_back(t_token **head, t_token *new_tok) {
  t_token *cur;

  if (!head || !new_tok)
    return;
  if (!*head) {
    *head = new_tok;
    return;
  }
  cur = *head;
  while (cur->next)
    cur = cur->next;
  cur->next = new_tok;
}

void token_list_free(t_token *head) {
  t_token *tmp;

  while (head) {
    tmp = head->next;
    free(head->value);
    free(head);
    head = tmp;
  }
}

int is_operator_char(char c) {
  return (c == '|' || c == '&' || c == ';' || c == '<' || c == '>');
}
