#ifndef BUILTINS_H
# define BUILTINS_H

# define FC_L 1
# define FC_N 2
# define FC_R 4
# define FC_S 8

typedef int	(*t_builtin)(char **);

typedef struct s_bidef
{
	const char	*name;
	t_builtin	fn;
}	t_bidef;

/*
** State of one fc invocation: options, operands and the resolved range
** (0-based positions in the history list).
*/
typedef struct s_fc
{
	int	opts;
	const char	*editor;
	const char	*subst;
	const char	*first;
	const char	*last;
	int	from;
	int	to;
	int	count;
}	t_fc;

t_builtin	builtin_find(const char *name);
const char	*builtin_name(int index);
int	opt_parse(char **argv, const char *valid, int *flags);
int	bi_cd(char **argv);
char	*path_canon(const char *path);
char	*cd_search(const char *dir, int *print);
int	bi_pwd(char **argv);
int	bi_echo(char **argv);
int	bi_exit(char **argv);
int	bi_type(char **argv);
int	bi_export(char **argv);
int	bi_unset(char **argv);
int	bi_set(char **argv);
int	set_option(const char *name, int on);
int	set_list_options(void);
char	*sh_quote(const char *value);
int	bi_alias(char **argv);
int	bi_unalias(char **argv);
int	bi_hash(char **argv);
int	bi_test(char **argv);
int	test_unary(const char *op, const char *arg);
int	test_binary(const char *left, const char *op, const char *right);
int	test_is_binary(const char *op);
int	bi_jobs(char **argv);
int	bi_fg(char **argv);
int	bi_bg(char **argv);
int	bi_wait(char **argv);
int	bi_kill(char **argv);
int	bi_fc(char **argv);
int	fc_parse(t_fc *fc, char **argv);
int	fc_range(t_fc *fc);
int	fc_resolve(t_fc *fc, const char *spec, int fallback);
int	fc_list(t_fc *fc);
int	fc_edit(t_fc *fc);
int	fc_reexec(t_fc *fc);
int	bi_history(char **argv);
int	bi_true(char **argv);
int	bi_false(char **argv);
int	bi_eval(char **argv);
int	bi_source(char **argv);
int	bi_exec(char **argv);
int	bi_read(char **argv);
int	bi_break(char **argv);
int	bi_continue(char **argv);
int	bi_return(char **argv);
int	bi_shift(char **argv);

#endif
