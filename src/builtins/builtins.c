#include "sh.h"

static const t_bidef	*builtin_table(void)
{
	static const t_bidef	table[] = {{".", bi_source}, {":", bi_true},
	{"[", bi_test}, {"alias", bi_alias}, {"bg", bi_bg},
	{"break", bi_break}, {"cd", bi_cd}, {"continue", bi_continue},
	{"echo", bi_echo}, {"eval", bi_eval}, {"exec", bi_exec},
	{"exit", bi_exit}, {"export", bi_export}, {"false", bi_false},
	{"fc", bi_fc}, {"fg", bi_fg}, {"hash", bi_hash},
	{"history", bi_history}, {"jobs", bi_jobs}, {"kill", bi_kill},
	{"pwd", bi_pwd}, {"read", bi_read}, {"return", bi_return},
	{"set", bi_set}, {"shift", bi_shift}, {"source", bi_source},
	{"test", bi_test}, {"true", bi_true}, {"type", bi_type},
	{"unalias", bi_unalias}, {"unset", bi_unset}, {"wait", bi_wait},
	{NULL, NULL}};

	return (table);
}

t_builtin	builtin_find(const char *name)
{
	const t_bidef	*table;
	int	i;

	table = builtin_table();
	i = 0;
	while (table[i].name && !str_eq(table[i].name, name))
		i++;
	return (table[i].fn);
}

/*
** Name of the builtin number index (alphabetical order), NULL past the
** last one. Used to enumerate the builtins.
*/
const char	*builtin_name(int index)
{
	const t_bidef	*table;
	int	i;

	table = builtin_table();
	i = 0;
	while (table[i].name && i < index)
		i++;
	return (table[i].name);
}

static int	opt_invalid(const char *who, char opt)
{
	dprintf(STDERR_FILENO, "42sh: %s: -%c: invalid option\n", who, opt);
	return (-1);
}

/*
** Parses the single-letter options at the start of argv. Each letter of
** valid owns one bit of *flags, in order. Returns the index of the first
** operand, or -1 after reporting an invalid option. "--" ends the
** options, a lone "-" is an operand.
*/
int	opt_parse(char **argv, const char *valid, int *flags)
{
	const char	*found;
	int	i;
	int	j;

	*flags = 0;
	i = 1;
	while (argv[i] && argv[i][0] == '-' && argv[i][1])
	{
		if (str_eq(argv[i], "--"))
			return (i + 1);
		j = 1;
		while (argv[i][j])
		{
			found = ft_strchr(valid, argv[i][j]);
			if (!found)
				return (opt_invalid(argv[0], argv[i][j]));
			*flags |= 1 << (found - valid);
			j++;
		}
		i++;
	}
	return (i);
}
