#include "sh.h"

/*
** Options of "set -o": the two line editing modes. Exactly one of them
** is active, so turning one off turns the other on.
*/
int	set_option(const char *name, int on)
{
	int	vi;

	if (str_eq(name, "vi"))
		vi = on;
	else if (str_eq(name, "emacs") || str_eq(name, "readline"))
		vi = !on;
	else
	{
		dprintf(STDERR_FILENO, "42sh: set: %s: invalid option name\n", name);
		return (1);
	}
	sh()->vi_mode = vi;
	if (sh()->interactive)
		line_set_mode(vi);
	return (0);
}

int	set_list_options(void)
{
	t_buf	out;

	buf_init(&out);
	if (sh()->vi_mode)
		buf_adds(&out, "emacs          \toff\nvi             \ton\n");
	else
		buf_adds(&out, "emacs          \ton\nvi             \toff\n");
	return (buf_write(&out, STDOUT_FILENO, "set"));
}
