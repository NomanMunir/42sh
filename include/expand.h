#ifndef EXPAND_H
# define EXPAND_H

# include "utils.h"

/*
** Every expanded character carries a mask byte:
**   M_LIT     unquoted literal (glob characters are active)
**   M_QUOTED  quoted, never split nor globbed
**   M_EXP     result of an unquoted expansion (field splitting applies)
**   M_MARK    placeholder left by a quote: forces the field to exist
**   M_SEP     hard field break (between the words of "$@")
*/
# define M_LIT 0
# define M_QUOTED 1
# define M_EXP 2
# define M_MARK 3
# define M_SEP 4

# define EXP_TILDE 1
# define EXP_ASSIGN 2
# define EXP_HEREDOC 4
# define EXP_PATTERN 8
# define EXP_PROCSUB 16

# define ARITH_MAX_DEPTH 64

typedef struct s_exp
{
	t_buf	out;
	t_buf	mask;
	int	flags;
	int	dq;
	int	err;
	int	no_params;
}	t_exp;

typedef struct s_field
{
	t_buf	txt;
	t_buf	pat;
	int	active;
	int	glob;
	int	ws_end;
	t_strv	*out;
	const char	*ifs;
}	t_field;

typedef struct s_arith
{
	const char	*s;
	const char	*expr;
	int	err;
	int	skip;
}	t_arith;

char	**expand_words(char **raw);
char	*expand_single(const char *raw, int flags);
void	exp_init(t_exp *e, int flags);
void	exp_free(t_exp *e);
void	exp_put(t_exp *e, char c, int mask);
void	exp_puts(t_exp *e, const char *s, int mask);
int	exp_mask(t_exp *e);
int	exp_lit_mask(t_exp *e);
void	exp_run(t_exp *e, const char *s);
size_t	exp_backslash(t_exp *e, const char *s);
size_t	exp_squote(t_exp *e, const char *s);
size_t	exp_dollar(t_exp *e, const char *s);
size_t	exp_backtick(t_exp *e, const char *s);
size_t	exp_tilde(t_exp *e, const char *s);
size_t	exp_procsub(t_exp *e, const char *s);
void	exp_param(t_exp *e, const char *name);
void	exp_brace(t_exp *e, const char *body, int len);
void	exp_cond(t_exp *e, const char *name, const char *op);
void	exp_trim(t_exp *e, const char *name, const char *op);
int	param_name_len(const char *s);
char	*cmdsub_run(const char *cmd);
void	procsub_close(void);
void	fields_split(t_exp *e, t_strv *out);
int	pat_match(const char *pat, const char *str);
int	pat_has_glob(const char *pat);
char	*pat_unescape(const char *pat);
char	**glob_expand(const char *pattern);
char	*arith_expand(const char *expr);
int	arith_eval(const char *expr, long *res);
long	arith_binary(t_arith *a, int min_prec);
long	arith_unary(t_arith *a);
long	arith_variable(t_arith *a);
long	arith_prefix(t_arith *a);
void	arith_space(t_arith *a);
void	arith_error(t_arith *a, const char *msg);

#endif
