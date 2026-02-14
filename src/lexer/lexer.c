/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include "token.h"

static int read_operator(const char *input, int i, t_token **tokens) {
  if (input[i] == '|' && input[i + 1] == '|')
    return (token_add_back(tokens, token_new(TOK_OR, "||")), i + 2);
  if (input[i] == '&' && input[i + 1] == '&')
    return (token_add_back(tokens, token_new(TOK_AND, "&&")), i + 2);
  if (input[i] == '>' && input[i + 1] == '>')
    return (token_add_back(tokens, token_new(TOK_REDIR_APPEND, ">>")), i + 2);
  if (input[i] == '<' && input[i + 1] == '<')
    return (token_add_back(tokens, token_new(TOK_HEREDOC, "<<")), i + 2);
  if (input[i] == '>' && input[i + 1] == '&')
    return (token_add_back(tokens, token_new(TOK_REDIR_OUT_FD, ">&")), i + 2);
  if (input[i] == '<' && input[i + 1] == '&')
    return (token_add_back(tokens, token_new(TOK_REDIR_IN_FD, "<&")), i + 2);
  if (input[i] == '|')
    return (token_add_back(tokens, token_new(TOK_PIPE, "|")), i + 1);
  if (input[i] == '&')
    return (token_add_back(tokens, token_new(TOK_AMP, "&")), i + 1);
  if (input[i] == ';')
    return (token_add_back(tokens, token_new(TOK_SEMI, ";")), i + 1);
  if (input[i] == '>')
    return (token_add_back(tokens, token_new(TOK_REDIR_OUT, ">")), i + 1);
  if (input[i] == '<')
    return (token_add_back(tokens, token_new(TOK_REDIR_IN, "<")), i + 1);
  return (i);
}

static char *word_append_char(char *word, char c) {
  char buf[2];
  char *result;

  buf[0] = c;
  buf[1] = '\0';
  if (!word)
    return (ft_strdup(buf));
  result = ft_strjoin(word, buf);
  free(word);
  return (result);
}

static int read_single_quote(const char *input, int i, char **word) {
  i++;
  while (input[i] && input[i] != '\'') {
    *word = word_append_char(*word, input[i]);
    i++;
  }
  if (input[i] == '\'')
    i++;
  return (i);
}

static int read_double_quote(const char *input, int i, char **word) {
  i++;
  while (input[i] && input[i] != '"') {
    if (input[i] == '\\' && input[i + 1] &&
        (input[i + 1] == '"' || input[i + 1] == '\\' || input[i + 1] == '$' ||
         input[i + 1] == '`')) {
      i++;
      *word = word_append_char(*word, input[i]);
    } else
      *word = word_append_char(*word, input[i]);
    i++;
  }
  if (input[i] == '"')
    i++;
  return (i);
}

static int read_word(const char *input, int i, t_token **tokens) {
  char *word;

  word = NULL;
  while (input[i] && input[i] != ' ' && input[i] != '\t' &&
         !is_operator_char(input[i])) {
    if (input[i] == '\'')
      i = read_single_quote(input, i, &word);
    else if (input[i] == '"')
      i = read_double_quote(input, i, &word);
    else if (input[i] == '\\' && input[i + 1]) {
      i++;
      word = word_append_char(word, input[i++]);
    } else
      word = word_append_char(word, input[i++]);
  }
  if (word) {
    token_add_back(tokens, token_new(TOK_WORD, word));
    free(word);
  }
  return (i);
}

t_token *lexer(const char *input) {
  t_token *tokens;
  int i;

  tokens = NULL;
  i = 0;
  while (input[i]) {
    if (input[i] == ' ' || input[i] == '\t') {
      i++;
      continue;
    }
    if (is_operator_char(input[i]))
      i = read_operator(input, i, &tokens);
    else
      i = read_word(input, i, &tokens);
  }
  token_add_back(&tokens, token_new(TOK_EOF, NULL));
  return (tokens);
}
