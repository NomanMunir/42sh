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

/*
** make_redir_op: create a redir token and stamp it with the explicit fd
** given by a digit prefix (e.g. the 2 in 2>&1), or -1 when there was none.
*/
static int make_redir_op(t_token **tokens, t_token_type type, const char *val,
                          int fd, int len, int i) {
  t_token *tok;

  tok = token_new(type, val);
  if (tok)
    tok->fd = fd;
  token_add_back(tokens, tok);
  return (i + len);
}

static int read_operator(const char *input, int i, t_token **tokens, int fd) {
  if (input[i] == '|' && input[i + 1] == '|')
    return (token_add_back(tokens, token_new(TOK_OR, "||")), i + 2);
  if (input[i] == '&' && input[i + 1] == '&')
    return (token_add_back(tokens, token_new(TOK_AND, "&&")), i + 2);
  if (input[i] == '>' && input[i + 1] == '>')
    return (make_redir_op(tokens, TOK_REDIR_APPEND, ">>", fd, 2, i));
  if (input[i] == '<' && input[i + 1] == '<')
    return (make_redir_op(tokens, TOK_HEREDOC, "<<", fd, 2, i));
  if (input[i] == '>' && input[i + 1] == '&')
    return (make_redir_op(tokens, TOK_REDIR_OUT_FD, ">&", fd, 2, i));
  if (input[i] == '<' && input[i + 1] == '&')
    return (make_redir_op(tokens, TOK_REDIR_IN_FD, "<&", fd, 2, i));
  if (input[i] == '|')
    return (token_add_back(tokens, token_new(TOK_PIPE, "|")), i + 1);
  if (input[i] == '&')
    return (token_add_back(tokens, token_new(TOK_AMP, "&")), i + 1);
  if (input[i] == ';')
    return (token_add_back(tokens, token_new(TOK_SEMI, ";")), i + 1);
  if (input[i] == '>')
    return (make_redir_op(tokens, TOK_REDIR_OUT, ">", fd, 1, i));
  if (input[i] == '<')
    return (make_redir_op(tokens, TOK_REDIR_IN, "<", fd, 1, i));
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
    *word = word_append_char(*word, '\x01');
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
      *word = word_append_char(*word, '\x01');
      *word = word_append_char(*word, input[i]);
    } else
      *word = word_append_char(*word, input[i]);
    i++;
  }
  if (input[i] == '"')
    i++;
  return (i);
}

/*
** read_cmd_sub: when we see $( in unquoted context,
** track paren depth and include everything up to and
** including the closing ).
*/
static int read_cmd_sub(const char *input, int i, char **word) {
  int depth;

  *word = word_append_char(*word, '$');
  *word = word_append_char(*word, '(');
  i += 2;
  depth = 1;
  while (input[i] && depth > 0) {
    if (input[i] == '(')
      depth++;
    else if (input[i] == ')')
      depth--;
    *word = word_append_char(*word, input[i]);
    i++;
  }
  return (i);
}

/*
** scan_redir_fd: if the run of digits starting at i is immediately
** followed by '<' or '>' (no space), it is an explicit fd prefix such
** as the 2 in "2>&1", not a word of its own. Returns the digit run
** length, or 0 if this position isn't a fd prefix.
*/
static int scan_redir_fd(const char *input, int i) {
  int start;

  start = i;
  while (ft_isdigit(input[i]))
    i++;
  if (i > start && (input[i] == '<' || input[i] == '>'))
    return (i - start);
  return (0);
}

static int parse_fd_number(const char *input, int start, int len) {
  int value;
  int i;

  value = 0;
  i = 0;
  while (i < len) {
    value = value * 10 + (input[start + i] - '0');
    i++;
  }
  return (value);
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
    else if (input[i] == '$' && input[i + 1] == '(')
      i = read_cmd_sub(input, i, &word);
    else if (input[i] == '\\' && input[i + 1]) {
      i++;
      word = word_append_char(word, '\x01');
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
  int fd_len;

  tokens = NULL;
  i = 0;
  while (input[i]) {
    if (input[i] == ' ' || input[i] == '\t') {
      i++;
      continue;
    }
    fd_len = scan_redir_fd(input, i);
    if (fd_len > 0) {
      i = read_operator(input, i + fd_len, &tokens,
                         parse_fd_number(input, i, fd_len));
    } else if (is_operator_char(input[i]))
      i = read_operator(input, i, &tokens, -1);
    else
      i = read_word(input, i, &tokens);
  }
  token_add_back(&tokens, token_new(TOK_EOF, NULL));
  return (tokens);
}
