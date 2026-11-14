#include "sh.h"

/*
** Appends the commands of the range to out, one per line. -r reverses
** the order; numbered adds the history numbers (0: no, 1: yes, 2: only
** the tab that follows them).
*/
void	fc_text(t_fc *fc, t_buf *out, int numbered)
{
	int	pos;
	int	last;
	int	step;

	pos = fc->from;
	last = fc->to;
	if (fc->opts & FC_R)
	{
		pos = fc->to;
		last = fc->from;
	}
	step = 1;
	if (pos > last)
		step = -1;
	last += step;
	while (pos != last)
	{
		if (numbered == 1)
			buf_addnum(out, hist_number(pos));
		if (numbered)
			buf_addc(out, '\t');
		buf_adds(out, hist_line(pos));
		buf_addc(out, '\n');
		pos += step;
	}
}

int	fc_list(t_fc *fc)
{
	t_buf	out;

	buf_init(&out);
	if (fc->opts & FC_N)
		fc_text(fc, &out, 2);
	else
		fc_text(fc, &out, 1);
	return (buf_write(&out, STDOUT_FILENO, "fc"));
}

/*
** Runs a command taken from the history (command is consumed). It
** replaces the fc command in the history, and is shown before it runs.
*/
static int	fc_run(char *command)
{
	int	status;

	hist_drop_last();
	dprintf(STDERR_FILENO, "%s\n", command);
	hist_add(command);
	status = sh_run_string(command);
	free(command);
	return (status);
}

/*
** Lets the user edit the commands of the range, then runs the result.
** Nothing runs if the editor fails.
*/
int	fc_edit(t_fc *fc)
{
	t_buf	text;
	char	*edited;

	buf_init(&text);
	fc_text(fc, &text, 0);
	edited = edit_text(text.s, fc->editor);
	buf_free(&text);
	if (!edited)
		return (1);
	return (fc_run(edited));
}

/*
** fc -s [old=new] [first]: runs a command again, after replacing every
** occurrence of old by new.
*/
int	fc_reexec(t_fc *fc)
{
	t_buf	out;
	const char	*line;
	size_t	old_len;

	line = hist_line(fc->from);
	old_len = 0;
	if (fc->subst)
		old_len = ft_strchr(fc->subst, '=') - fc->subst;
	buf_init(&out);
	while (*line)
	{
		if (old_len > 0 && ft_strncmp(line, fc->subst, old_len) == 0)
		{
			buf_adds(&out, fc->subst + old_len + 1);
			line += old_len;
		}
		else
			buf_addc(&out, *line++);
	}
	return (fc_run(buf_take(&out)));
}
