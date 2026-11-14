#include "sh.h"

/*
** A variable holds an expression: an unset or empty one is worth 0.
*/
static long	arith_var_get(t_arith *a, const char *name)
{
	const char	*text;
	long	value;

	text = var_get(name);
	if (!text || !*text)
		return (0);
	if (arith_eval(text, &value) < 0)
		a->err = 1;
	return (value);
}

static void	arith_var_set(t_arith *a, const char *name, long value)
{
	char	*text;

	if (a->skip || a->err)
		return ;
	text = xitoa(value);
	var_set(name, text, 0);
	free(text);
}

/*
** Returns +1 or -1 if a postfix "++" or "--" follows (and consumes it).
** "a++b" is read as "a + +b": the step must not be followed by an operand.
*/
static int	postfix_step(t_arith *a)
{
	const char	*s;

	arith_space(a);
	s = a->s;
	if (!((s[0] == '+' && s[1] == '+') || (s[0] == '-' && s[1] == '-')))
		return (0);
	s += 2;
	while (*s == ' ' || *s == '\t')
		s++;
	if (ft_isalnum(*s) || *s == '_' || *s == '(' || *s == '$')
		return (0);
	a->s += 2;
	if (a->s[-1] == '+')
		return (1);
	return (-1);
}

/*
** NAME, NAME++ or NAME--.
*/
long	arith_variable(t_arith *a)
{
	char	*name;
	long	value;
	size_t	len;
	int	step;

	len = name_len(a->s);
	name = xstrndup(a->s, len);
	a->s += len;
	value = arith_var_get(a, name);
	step = postfix_step(a);
	if (step)
		arith_var_set(a, name, value + step);
	free(name);
	return (value);
}

/*
** ++NAME or --NAME.
*/
long	arith_prefix(t_arith *a)
{
	char	*name;
	long	value;
	size_t	len;
	int	step;

	step = 1;
	if (*a->s == '-')
		step = -1;
	a->s += 2;
	arith_space(a);
	len = name_len(a->s);
	name = xstrndup(a->s, len);
	a->s += len;
	value = arith_var_get(a, name) + step;
	arith_var_set(a, name, value);
	free(name);
	return (value);
}
