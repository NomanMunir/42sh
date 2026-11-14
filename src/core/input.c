#include "sh.h"

/*
** Next line of a string input; the string pointer moves past it.
*/
static char	*line_from_str(t_input *in)
{
	size_t	len;
	char	*line;

	if (!in->str || !*in->str)
		return (NULL);
	len = strcspn(in->str, "\n");
	line = xstrndup(in->str, len);
	in->str += len;
	if (*in->str == '\n')
		in->str++;
	return (line);
}

/*
** Next line of a descriptor, read byte by byte: nothing is consumed
** beyond the newline, so the commands that share the descriptor (read,
** cat...) find the rest of the input where they expect it.
*/
static char	*line_from_fd(t_input *in)
{
	t_buf	buf;
	ssize_t	n;
	char	c;

	buf_init(&buf);
	c = '\0';
	n = read(in->fd, &c, 1);
	while ((n > 0 && c != '\n') || (n < 0 && errno == EINTR))
	{
		if (n > 0 && c != '\0')
			buf_addc(&buf, c);
		n = read(in->fd, &c, 1);
	}
	if (n <= 0 && buf.len == 0)
	{
		buf_free(&buf);
		return (NULL);
	}
	return (buf_take(&buf));
}

/*
** Next line typed at the terminal, after history expansion. A line whose
** expansion failed is dropped (an empty line is run instead).
*/
static char	*line_from_tty(int cont)
{
	char	*line;
	char	*expanded;
	int	changed;

	line = line_read(cont);
	if (!line)
		return (NULL);
	changed = 0;
	expanded = hist_expand(line, &changed);
	free(line);
	if (!expanded)
	{
		sh()->status = 1;
		return (xstrdup(""));
	}
	if (changed)
		dprintf(STDERR_FILENO, "%s\n", expanded);
	return (expanded);
}

/*
** Returns the next line of the input without its newline, NULL at the
** end. cont tells the terminal reader to show the continuation prompt.
*/
char	*input_line(t_input *in, int cont)
{
	if (in->kind == IN_TTY)
		return (line_from_tty(cont));
	if (in->kind == IN_FD)
		return (line_from_fd(in));
	return (line_from_str(in));
}
