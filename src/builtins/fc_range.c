#include "sh.h"

/*
** Position (0-based) of the command designated by spec: a history
** number, a negative offset from the latest command, or the beginning
** of a command line. Numbers out of range are brought back to the
** nearest end. Returns -1 when no command starts with the string.
*/
int	fc_resolve(t_fc *fc, const char *spec, int fallback)
{
	long	number;
	int	pos;

	if (!spec)
		return (fallback);
	if (parse_long(spec, &number))
	{
		if (number <= 0)
			number += fc->count;
		else
			number -= hist_number(0);
		if (number < 0)
			number = 0;
		if (number >= fc->count)
			number = fc->count - 1;
		return ((int)number);
	}
	pos = fc->count - 1;
	while (pos >= 0 && ft_strncmp(hist_line(pos), spec, ft_strlen(spec)) != 0)
		pos--;
	if (pos < 0)
		dprintf(STDERR_FILENO, "42sh: fc: %s: no command found\n", spec);
	return (pos);
}

/*
** Works out the range of commands fc applies to. The fc command line
** itself is the newest entry of the history and is left out. Defaults:
** the last 16 commands when listing, the previous command otherwise.
*/
int	fc_range(t_fc *fc)
{
	int	start;

	fc->count = hist_count() - 1;
	if (fc->count <= 0)
	{
		dprintf(STDERR_FILENO, "42sh: fc: no command found\n");
		return (-1);
	}
	start = fc->count - 1;
	if ((fc->opts & FC_L) && !(fc->opts & FC_S))
		start = fc->count - 16;
	if (start < 0)
		start = 0;
	fc->from = fc_resolve(fc, fc->first, start);
	if ((fc->opts & FC_L) && !(fc->opts & FC_S))
		fc->to = fc_resolve(fc, fc->last, fc->count - 1);
	else
		fc->to = fc_resolve(fc, fc->last, fc->from);
	if (fc->from < 0 || fc->to < 0)
		return (-1);
	return (0);
}
