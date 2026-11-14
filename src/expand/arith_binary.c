#include "sh.h"

/*
** Precedence of the binary operator at s (0 if there is none); len
** receives its length.
*/
static int	binary_op(const char *s, int *len)
{
	static const char	*ops[] = {"||", "&&", "==", "!=", "<=", ">=", "<",
		">", "+", "-", "*", "/", "%", NULL};
	static const int	prec[] = {1, 2, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 6};
	int	i;

	i = 0;
	while (ops[i] && ft_strncmp(s, ops[i], ft_strlen(ops[i])) != 0)
		i++;
	if (!ops[i])
		return (0);
	*len = ft_strlen(ops[i]);
	return (prec[i]);
}

static long	arith_divide(t_arith *a, char op, long left, long right)
{
	if (right == 0)
	{
		if (!a->skip)
			arith_error(a, "division by 0");
		return (0);
	}
	if (left == LONG_MIN && right == -1)
	{
		if (op == '/')
			return (LONG_MIN);
		return (0);
	}
	if (op == '/')
		return (left / right);
	return (left % right);
}

/*
** Arithmetic wraps around like the usual two's complement machine.
*/
static long	arith_apply(t_arith *a, const char *op, long left, long right)
{
	if (op[0] == '+')
		return ((long)((unsigned long)left + (unsigned long)right));
	if (op[0] == '-')
		return ((long)((unsigned long)left - (unsigned long)right));
	if (op[0] == '*')
		return ((long)((unsigned long)left * (unsigned long)right));
	if (op[0] == '/' || op[0] == '%')
		return (arith_divide(a, op[0], left, right));
	if (op[0] == '=')
		return (left == right);
	if (op[0] == '!')
		return (left != right);
	if (op[0] == '<' && op[1] == '=')
		return (left <= right);
	if (op[0] == '>' && op[1] == '=')
		return (left >= right);
	if (op[0] == '<')
		return (left < right);
	return (left > right);
}

/*
** && and ||: the right side is still parsed when the left side decides
** the result, but its side effects (++, division errors) are skipped.
*/
static long	arith_logical(t_arith *a, const char *op, long left, int prec)
{
	long	right;
	int	decided;

	decided = ((op[0] == '&' && !left) || (op[0] == '|' && left));
	a->skip += decided;
	right = arith_binary(a, prec + 1);
	a->skip -= decided;
	if (op[0] == '&')
		return (left && right);
	return (left || right);
}

/*
** Precedence climbing over the binary operators.
*/
long	arith_binary(t_arith *a, int min_prec)
{
	long	left;
	const char	*op;
	int	len;
	int	prec;

	left = arith_unary(a);
	while (!a->err)
	{
		arith_space(a);
		op = a->s;
		prec = binary_op(op, &len);
		if (prec == 0 || prec < min_prec)
			break ;
		a->s += len;
		if (prec <= 2)
			left = arith_logical(a, op, left, prec);
		else
			left = arith_apply(a, op, left, arith_binary(a, prec + 1));
	}
	return (left);
}
