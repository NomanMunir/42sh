#include "sh.h"

/*
** ${parameter:=word}: assigns the expansion of word, then substitutes it.
*/
static void	cond_assign(t_exp *e, const char *name, const char *word)
{
	char	*value;

	if (!is_name(name))
	{
		dprintf(STDERR_FILENO, "42sh: $%s: cannot assign in this way\n",
			name);
		e->err = 1;
		return ;
	}
	value = expand_single(word, EXP_TILDE);
	if (!value)
	{
		e->err = 1;
		return ;
	}
	var_set(name, value, 0);
	exp_puts(e, value, exp_mask(e));
	free(value);
}

/*
** ${parameter:?word}: prints word (or a default message) and fails the
** command; a non-interactive shell exits.
*/
static void	cond_error(t_exp *e, const char *name, const char *word)
{
	char	*message;

	message = expand_single(word, EXP_TILDE);
	if (message && *message)
		dprintf(STDERR_FILENO, "42sh: %s: %s\n", name, message);
	else
		dprintf(STDERR_FILENO, "42sh: %s: parameter null or not set\n",
			name);
	free(message);
	e->err = 1;
	if (!sh()->interactive)
	{
		sh()->status = 1;
		sh()->unwind = U_EXIT;
	}
}

/*
** The - = ? + family. op points at the optional colon; with the colon an
** empty value counts as missing too.
*/
void	exp_cond(t_exp *e, const char *name, const char *op)
{
	char	*value;
	int	missing;

	value = param_value(name);
	missing = (!value || (op[0] == ':' && !*value));
	if (op[0] == ':')
		op++;
	if (op[0] == '+')
	{
		if (!missing)
			exp_run(e, op + 1);
	}
	else if (!missing)
		exp_puts(e, value, exp_mask(e));
	else if (op[0] == '-')
		exp_run(e, op + 1);
	else if (op[0] == '=')
		cond_assign(e, name, op + 1);
	else
		cond_error(e, name, op + 1);
	free(value);
}
