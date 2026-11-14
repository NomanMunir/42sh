#include "sh.h"

/*
** One operand of kill -l: a number gives the signal name (a status above
** 128 is that of a process killed by a signal), a name gives the number.
*/
static int	kill_list_one(t_buf *out, const char *arg)
{
	int	number;

	number = sig_number(arg);
	if (number > 128)
		number -= 128;
	if (number < 0 || !sig_name(number))
	{
		dprintf(STDERR_FILENO, "42sh: kill: %s: invalid signal "
			"specification\n", arg);
		return (1);
	}
	if (ft_isdigit(arg[0]))
		buf_adds(out, sig_name(number));
	else
		buf_addnum(out, number);
	buf_addc(out, '\n');
	return (0);
}

/*
** kill -l [exit_status...]: without operand, every signal name.
*/
int	kill_list(char **args)
{
	t_buf	out;
	int	status;
	int	i;

	buf_init(&out);
	status = 0;
	i = 0;
	while (!args[0] && sig_name(i + 1))
	{
		buf_adds(&out, sig_name(i + 1));
		buf_addc(&out, '\n');
		i++;
	}
	while (args[i])
		status |= kill_list_one(&out, args[i++]);
	if (buf_write(&out, STDOUT_FILENO, "kill"))
		return (1);
	return (status);
}
