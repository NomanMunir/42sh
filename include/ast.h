#ifndef AST_H
# define AST_H

# include "lexer.h"

# define PARSE_MAX_DEPTH 500

/*
** Node layout by type:
**   N_CMD       words = command words, assigns = NAME=value prefixes
**   N_PIPE/N_AND/N_OR/N_SEQ   left, right
**   N_BG/N_NOT/N_SUBSHELL/N_GROUP   left = body
**   N_IF        left = condition, right = then, other = else / elif
**   N_WHILE/N_UNTIL   left = condition, right = body
**   N_FOR       words[0] = variable, words[1..] = list, right = body
**   N_CASE      words[0] = subject, left = first N_CASEITEM
**   N_CASEITEM  words = patterns, left = body, next = next item
**   N_FUNC      words[0] = name, left = body
*/
typedef enum e_ntype
{
	N_CMD,
	N_PIPE,
	N_AND,
	N_OR,
	N_SEQ,
	N_BG,
	N_NOT,
	N_SUBSHELL,
	N_GROUP,
	N_IF,
	N_WHILE,
	N_UNTIL,
	N_FOR,
	N_CASE,
	N_CASEITEM,
	N_FUNC
}	t_ntype;

typedef struct s_redir
{
	t_tok	op;
	int	fd;
	char	*word;
	char	*body;
	int	quoted;
	struct s_redir	*next;
}	t_redir;

typedef struct s_node
{
	t_ntype	type;
	char	**words;
	char	**assigns;
	t_redir	*redirs;
	struct s_node	*left;
	struct s_node	*right;
	struct s_node	*other;
	struct s_node	*next;
	char	*text;
}	t_node;

typedef struct s_parser
{
	t_token	*tok;
	int	status;
	int	eof_ok;
	int	depth;
}	t_parser;

int	parse_text(const char *text, int eof_ok, t_node **out);
t_node	*parse_list(t_parser *p);
t_node	*parse_body(t_parser *p);
t_node	*parse_command(t_parser *p);
t_node	*parse_simple(t_parser *p);
t_node	*parse_compound(t_parser *p);
t_node	*parse_if(t_parser *p);
t_node	*parse_loop(t_parser *p);
t_node	*parse_for(t_parser *p);
t_node	*parse_case(t_parser *p);
void	parse_redir(t_parser *p, t_node *n);
void	p_error(t_parser *p);
void	p_next(t_parser *p);
void	p_skip_newlines(t_parser *p);
int	p_is_word(t_parser *p, const char *word);
void	p_expect(t_parser *p, const char *word);
int	is_reserved(const char *word);
int	p_at_list_end(t_parser *p);
int	alias_expand_at(t_token *tok);
t_node	*node_new(t_ntype type);
t_node	*node_join(t_ntype type, t_node *left, t_node *right);
t_node	*node_wrap(t_ntype type, t_node *child);
t_node	*node_dup(const t_node *n);
void	node_free(t_node *n);
void	redir_free(t_redir *r);

#endif
