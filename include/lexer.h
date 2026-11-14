#ifndef LEXER_H
# define LEXER_H

# include "utils.h"

# define PARSE_OK 0
# define PARSE_MORE 1
# define PARSE_ERR 2
# define PARSE_ABORT 3

# define TF_ALIASABLE 1

/*
** Token types. Every type from T_LESS on is a redirection operator.
*/
typedef enum e_tok
{
	T_WORD,
	T_NEWLINE,
	T_EOF,
	T_PIPE,
	T_AND_IF,
	T_OR_IF,
	T_SEMI,
	T_DSEMI,
	T_AMP,
	T_LPAREN,
	T_RPAREN,
	T_LESS,
	T_GREAT,
	T_DGREAT,
	T_DLESS,
	T_DLESSDASH,
	T_LESSAND,
	T_GREATAND,
	T_LESSGREAT,
	T_CLOBBER
}	t_tok;

/*
** str:     raw text of the token (quotes are kept for the expander)
** fd:      explicit io number in front of a redirection, -1 if none
** heredoc: body of a here-document, stored on its operator token
** chain:   ":a:b:" list of aliases this token was expanded from
*/
typedef struct s_token
{
	t_tok	type;
	char	*str;
	int	fd;
	char	*heredoc;
	int	hd_quoted;
	int	flags;
	char	*chain;
	struct s_token	*next;
}	t_token;

typedef struct s_lexer
{
	const char	*src;
	size_t	pos;
	t_token	*head;
	t_token	*tail;
	t_token	*hd_from;
	int	status;
	int	eof_ok;
}	t_lexer;

t_token	*lex(const char *src, int eof_ok, int *status);
int	lex_operator(t_lexer *lx, int fd);
void	lex_word(t_lexer *lx);
void	lex_heredocs(t_lexer *lx);
t_token	*tok_add(t_lexer *lx, t_tok type, const char *str);
void	tok_free(t_token *tok);
void	tok_free_list(t_token *tok);
char	*tokens_text(t_token *tok, t_token *end);
int	is_redir(t_tok type);
char	*unquote(const char *raw);
int	scan_unit(const char *s);
int	scan_squote(const char *s);
int	scan_dquote(const char *s);
int	scan_backtick(const char *s);
int	scan_subst(const char *s);
int	scan_group(const char *s);

#endif
