#include "sh.h"

static long	arith_number(t_arith *a)
{
	char	*end;
	long	value;

	errno = 0;
	value = strtol(a->s, &end, 0);
	if (ft_isalnum(*end) || *end == '_')
		arith_error(a, "invalid number");
	a->s = end;
	return (value);
}

static long	arith_primary(t_arith *a)
{
	long	value;

	arith_space(a);
	if (*a->s == '(')
	{
		a->s++;
		value = arith_binary(a, 1);
		arith_space(a);
		if (*a->s == ')')
			a->s++;
		else
			arith_error(a, "missing `)'");
		return (value);
	}
	if (ft_isdigit(*a->s))
		return (arith_number(a));
	if (name_len(a->s) > 0)
		return (arith_variable(a));
	arith_error(a, "operand expected");
	return (0);
}

/*
** "++" or "--" is a prefix operator only when a variable name follows.
*/
static int	is_prefix_step(const char *s)
{
	if (!((s[0] == '+' && s[1] == '+') || (s[0] == '-' && s[1] == '-')))
		return (0);
	s += 2;
	while (*s == ' ' || *s == '\t')
		s++;
	return (name_len(s) > 0);
}

/*
** unary : ('+' | '-' | '!') unary | ('++' | '--') NAME | primary
*/
long	arith_unary(t_arith *a)
{
	char	op;

	arith_space(a);
	if (is_prefix_step(a->s))
		return (arith_prefix(a));
	op = *a->s;
	if (op == '+' || op == '-' || (op == '!' && a->s[1] != '='))
	{
		a->s++;
		if (op == '+')
			return (arith_unary(a));
		if (op == '-')
			return ((long)(0UL - (unsigned long)arith_unary(a)));
		return (!arith_unary(a));
	}
	return (arith_primary(a));
}
