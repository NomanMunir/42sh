#include "sh.h"

static int	history_show(int count)
{
	t_buf	out;
	int	pos;

	pos = 0;
	if (count < hist_count())
		pos = hist_count() - count;
	buf_init(&out);
	while (pos < hist_count())
	{
		buf_addnum(&out, hist_number(pos));
		buf_adds(&out, "  ");
		buf_adds(&out, hist_line(pos));
		buf_addc(&out, '\n');
		pos++;
	}
	return (buf_write(&out, STDOUT_FILENO, "history"));
}

/*
** history [n]: lists the history (the last n entries).
** history -c: forgets it.
*/
int	bi_history(char **argv)
{
	long	count;
	int	flags;
	int	i;

	i = opt_parse(argv, "c", &flags);
	if (i < 0)
		return (2);
	if (flags)
	{
		hist_clear();
		return (0);
	}
	count = INT_MAX;
	if (argv[i] && (!parse_long(argv[i], &count) || count < 0))
	{
		dprintf(STDERR_FILENO, "42sh: history: %s: numeric argument "
			"required\n", argv[i]);
		return (1);
	}
	if (count > INT_MAX)
		count = INT_MAX;
	return (history_show((int)count));
}
