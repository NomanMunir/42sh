#include "sh.h"

/*
** Stores one character of the line (text in line[0], and in line[1] a
** mark telling whether a backslash protected it). Returns 1 when the
** line is complete.
*/
static int	read_char(t_buf *line, char c, int *escaped, int raw)
{
	if (c == '\n' && !*escaped)
		return (1);
	if (c == '\\' && !raw && !*escaped)
	{
		*escaped = 1;
		return (0);
	}
	if (c != '\n' && c != '\0')
	{
		buf_addc(&line[0], c);
		buf_addc(&line[1], '0' + *escaped);
	}
	*escaped = 0;
	return (0);
}

/*
** Reads one logical line from the standard input, byte by byte so that
** nothing past the newline is consumed. Returns 1, 0 at end of input,
** -1 on error or interrupt.
*/
static int	read_line(t_buf *line, int raw)
{
	ssize_t	n;
	char	c;
	int	escaped;
	int	done;

	escaped = 0;
	done = 0;
	while (!done)
	{
		n = read(STDIN_FILENO, &c, 1);
		if (n == 0 || (n < 0 && (errno != EINTR || sig_peek() == SIGINT)))
			return ((int)n);
		if (n > 0)
			done = read_char(line, c, &escaped, raw);
	}
	return (1);
}

static int	read_names_valid(char **names)
{
	int	i;

	i = 0;
	while (names[i] && is_name(names[i]))
		i++;
	if (!names[i])
		return (1);
	dprintf(STDERR_FILENO, "42sh: read: `%s': not a valid identifier\n",
		names[i]);
	return (0);
}

/*
** read [-r] [name...]: reads a line, splits it into fields and assigns
** them to the names (REPLY by default). Status 1 at end of input.
*/
int	bi_read(char **argv)
{
	static char	*reply[] = {"REPLY", NULL};
	t_buf	line[2];
	int	flags;
	int	first;
	int	got;

	first = opt_parse(argv, "r", &flags);
	if (first < 0 || !read_names_valid(argv + first))
		return (2);
	buf_init(&line[0]);
	buf_init(&line[1]);
	got = read_line(line, flags);
	if (argv[first])
		read_assign(argv + first, line);
	else
		read_assign(reply, line);
	buf_free(&line[0]);
	buf_free(&line[1]);
	if (got < 0 && sig_peek() == SIGINT)
		return (128 + SIGINT);
	return (got <= 0);
}
