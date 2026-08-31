/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_arith.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: 42sh                                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                     #+#    #+#             */
/*                                                    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

/*
** Recursive-descent evaluator for $((expr)), covering exactly the
** operators listed by the subject: ++ -- + - * / % <= >= < > == != && ||.
** Grammar (low to high precedence):
**   or -> and ('||' and)*
**   and -> eq ('&&' eq)*
**   eq -> rel (('==' | '!=') rel)*
**   rel -> add (('<=' | '>=' | '<' | '>') add)*
**   add -> term (('+' | '-') term)*
**   term -> unary (('*' | '/' | '%') unary)*
**   unary -> ('+' | '-' | '!') unary | primary
**   primary -> NUMBER | ('++' | '--') NAME | NAME (('++' | '--')?)
**              | '(' or ')'
*/

static int parse_or(const char *s, int *i, t_shell *shell);

static void skip_ws(const char *s, int *i) {
  while (s[*i] == ' ' || s[*i] == '\t')
    (*i)++;
}

static int get_ident_len(const char *s, int i) {
  int len;

  if (!(ft_isalpha(s[i]) || s[i] == '_'))
    return (0);
  len = 0;
  while (ft_isalnum(s[i + len]) || s[i + len] == '_')
    len++;
  return (len);
}

static int var_get_int(t_shell *shell, const char *name) {
  char *value;

  value = var_get(shell, name);
  if (!value || !*value)
    return (0);
  return (ft_atoi(value));
}

static void var_set_int(t_shell *shell, const char *name, int value) {
  char *str;

  str = ft_itoa(value);
  var_set(shell, name, str);
  free(str);
}

static int parse_inc_dec_prefix(const char *s, int *i, t_shell *shell,
                                 int delta) {
  char *name;
  int len;
  int value;

  *i += 2;
  skip_ws(s, i);
  len = get_ident_len(s, *i);
  name = ft_substr(s, *i, len);
  *i += len;
  value = var_get_int(shell, name) + delta;
  var_set_int(shell, name, value);
  free(name);
  return (value);
}

static int parse_ident(const char *s, int *i, t_shell *shell, int len) {
  char *name;
  int value;

  name = ft_substr(s, *i, len);
  *i += len;
  value = var_get_int(shell, name);
  if (s[*i] == '+' && s[*i + 1] == '+') {
    *i += 2;
    var_set_int(shell, name, value + 1);
  } else if (s[*i] == '-' && s[*i + 1] == '-') {
    *i += 2;
    var_set_int(shell, name, value - 1);
  }
  free(name);
  return (value);
}

static int parse_primary(const char *s, int *i, t_shell *shell) {
  int value;
  int len;

  skip_ws(s, i);
  if (s[*i] == '(') {
    (*i)++;
    value = parse_or(s, i, shell);
    skip_ws(s, i);
    if (s[*i] == ')')
      (*i)++;
    return (value);
  }
  if (s[*i] == '+' && s[*i + 1] == '+')
    return (parse_inc_dec_prefix(s, i, shell, 1));
  if (s[*i] == '-' && s[*i + 1] == '-')
    return (parse_inc_dec_prefix(s, i, shell, -1));
  if (ft_isdigit(s[*i])) {
    value = 0;
    while (ft_isdigit(s[*i]))
      value = value * 10 + (s[(*i)++] - '0');
    return (value);
  }
  len = get_ident_len(s, *i);
  if (len > 0)
    return (parse_ident(s, i, shell, len));
  return (0);
}

static int parse_unary(const char *s, int *i, t_shell *shell) {
  skip_ws(s, i);
  if (s[*i] == '+' && s[*i + 1] != '+') {
    (*i)++;
    return (parse_unary(s, i, shell));
  }
  if (s[*i] == '-' && s[*i + 1] != '-') {
    (*i)++;
    return (-parse_unary(s, i, shell));
  }
  if (s[*i] == '!') {
    (*i)++;
    return (!parse_unary(s, i, shell));
  }
  return (parse_primary(s, i, shell));
}

static int parse_term(const char *s, int *i, t_shell *shell) {
  int value;
  int rhs;

  value = parse_unary(s, i, shell);
  skip_ws(s, i);
  while (s[*i] == '*' || s[*i] == '/' || s[*i] == '%') {
    if (s[*i] == '*') {
      (*i)++;
      value *= parse_unary(s, i, shell);
    } else if (s[*i] == '/') {
      (*i)++;
      rhs = parse_unary(s, i, shell);
      value = rhs ? value / rhs : 0;
    } else {
      (*i)++;
      rhs = parse_unary(s, i, shell);
      value = rhs ? value % rhs : 0;
    }
    skip_ws(s, i);
  }
  return (value);
}

static int parse_additive(const char *s, int *i, t_shell *shell) {
  int value;

  value = parse_term(s, i, shell);
  skip_ws(s, i);
  while (s[*i] == '+' || s[*i] == '-') {
    if (s[*i] == '+') {
      (*i)++;
      value += parse_term(s, i, shell);
    } else {
      (*i)++;
      value -= parse_term(s, i, shell);
    }
    skip_ws(s, i);
  }
  return (value);
}

static int parse_relational(const char *s, int *i, t_shell *shell) {
  int value;

  value = parse_additive(s, i, shell);
  skip_ws(s, i);
  while (1) {
    if (s[*i] == '<' && s[*i + 1] == '=') {
      *i += 2;
      value = (value <= parse_additive(s, i, shell));
    } else if (s[*i] == '>' && s[*i + 1] == '=') {
      *i += 2;
      value = (value >= parse_additive(s, i, shell));
    } else if (s[*i] == '<') {
      (*i)++;
      value = (value < parse_additive(s, i, shell));
    } else if (s[*i] == '>') {
      (*i)++;
      value = (value > parse_additive(s, i, shell));
    } else
      break;
    skip_ws(s, i);
  }
  return (value);
}

static int parse_equality(const char *s, int *i, t_shell *shell) {
  int value;

  value = parse_relational(s, i, shell);
  skip_ws(s, i);
  while ((s[*i] == '=' && s[*i + 1] == '=') ||
         (s[*i] == '!' && s[*i + 1] == '=')) {
    if (s[*i] == '=') {
      *i += 2;
      value = (value == parse_relational(s, i, shell));
    } else {
      *i += 2;
      value = (value != parse_relational(s, i, shell));
    }
    skip_ws(s, i);
  }
  return (value);
}

static int parse_and(const char *s, int *i, t_shell *shell) {
  int value;
  int rhs;

  value = parse_equality(s, i, shell);
  skip_ws(s, i);
  while (s[*i] == '&' && s[*i + 1] == '&') {
    *i += 2;
    rhs = parse_equality(s, i, shell);
    value = value && rhs;
    skip_ws(s, i);
  }
  return (value);
}

static int parse_or(const char *s, int *i, t_shell *shell) {
  int value;
  int rhs;

  value = parse_and(s, i, shell);
  skip_ws(s, i);
  while (s[*i] == '|' && s[*i + 1] == '|') {
    *i += 2;
    rhs = parse_and(s, i, shell);
    value = value || rhs;
    skip_ws(s, i);
  }
  return (value);
}

char *expand_arithmetic(const char *expr, t_shell *shell) {
  int i;
  int value;

  i = 0;
  value = parse_or(expr, &i, shell);
  return (ft_itoa(value));
}
