#include "sh.h"

#define ECHO_N 1
#define ECHO_E 2

/*
** An argument is an option only if it is made of the letters n, e, E.
*/
static int	echo_option(const char *arg, int *flags)
{
	int	i;
	int	found;

	if (arg[0] != '-' || !arg[1])
		return (0);
	i = 1;
	while (arg[i] && ft_strchr("neE", arg[i]))
		i++;
	if (arg[i])
		return (0);
	found = *flags;
	i = 1;
	while (arg[i])
	{
		if (arg[i] == 'n')
			found |= ECHO_N;
		else if (arg[i] == 'e')
			found |= ECHO_E;
		else
			found &= ~ECHO_E;
		i++;
	}
	*flags = found;
	return (1);
}

/*
** \0nnn: up to three octal digits. Returns the number of characters used
** after the backslash.
*/
static int	echo_octal(t_buf *out, const char *s)
{
	int	value;
	int	i;

	value = 0;
	i = 1;
	while (i < 4 && s[i] >= '0' && s[i] <= '7')
		value = value * 8 + (s[i++] - '0');
	buf_addc(out, (char)value);
	return (i);
}

/*
** Appends s with its backslash sequences decoded (echo -e). Returns 0
** when "\c" asks to stop all output.
*/
static int	echo_escaped(t_buf *out, const char *s)
{
	static const char	*names = "abefnrtv\\";
	static const char	*values = "\a\b\033\f\n\r\t\v\\";
	const char	*found;

	while (*s)
	{
		found = NULL;
		if (s[0] == '\\' && s[1])
			found = ft_strchr(names, s[1]);
		if (s[0] == '\\' && s[1] == 'c')
			return (0);
		if (s[0] == '\\' && s[1] == '0')
			s += echo_octal(out, s + 1);
		else if (found)
			buf_addc(out, values[found - names]);
		else
			buf_addc(out, *s);
		if (found)
			s++;
		s++;
	}
	return (1);
}

/*
** echo [-neE] [string...]
*/
int	bi_echo(char **argv)
{
	t_buf	out;
	int	flags;
	int	more;
	int	i;

	flags = 0;
	i = 1;
	while (argv[i] && echo_option(argv[i], &flags))
		i++;
	buf_init(&out);
	more = 1;
	while (argv[i] && more)
	{
		if (flags & ECHO_E)
			more = echo_escaped(&out, argv[i]);
		else
			buf_adds(&out, argv[i]);
		if (argv[++i] && more)
			buf_addc(&out, ' ');
	}
	if (more && !(flags & ECHO_N))
		buf_addc(&out, '\n');
	return (buf_write(&out, STDOUT_FILENO, "echo"));
}
