#include "sh.h"

void	arith_space(t_arith *a)
{
	while (*a->s == ' ' || *a->s == '\t' || *a->s == '\n')
		a->s++;
}

void	arith_error(t_arith *a, const char *msg)
{
	if (!a->err)
		dprintf(STDERR_FILENO, "42sh: %s: %s\n", a->expr, msg);
	a->err = 1;
}

/*
** Evaluates an arithmetic expression. Returns 0 and stores the value in
** *res, or -1 after printing an error.
*/
int	arith_eval(const char *expr, long *res)
{
	t_arith	a;

	a.s = expr;
	a.expr = expr;
	a.err = 0;
	a.skip = 0;
	*res = 0;
	arith_space(&a);
	if (!*a.s)
		return (0);
	if (++sh()->arith_depth > ARITH_MAX_DEPTH)
		arith_error(&a, "expression recursion level exceeded");
	else
	{
		*res = arith_binary(&a, 1);
		arith_space(&a);
		if (!a.err && *a.s)
			arith_error(&a, "syntax error in expression");
	}
	sh()->arith_depth--;
	return (-a.err);
}

/*
** $((expression)): parameters and command substitutions inside the
** expression are expanded first, then the result is computed.
*/
char	*arith_expand(const char *expr)
{
	char	*text;
	long	value;
	int	failed;

	text = expand_single(expr, EXP_HEREDOC);
	if (!text)
		return (NULL);
	failed = arith_eval(text, &value);
	free(text);
	if (failed)
		return (NULL);
	return (xitoa(value));
}
