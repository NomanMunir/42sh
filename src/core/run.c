#include "sh.h"

/*
** Parses the command held in text, reading the lines it still needs
** (open quote, here-document, unfinished compound command...). When the
** input ends first, the text is parsed one last time as it is.
*/
static int	read_command(t_input *in, t_buf *text, t_node **tree)
{
	char	*more;
	int	status;

	status = parse_text(text->s, 0, tree);
	while (status == PARSE_MORE)
	{
		more = input_line(in, 1);
		if (in->kind == IN_TTY && sig_take() == SIGINT)
		{
			free(more);
			return (PARSE_ABORT);
		}
		if (!more)
			return (parse_text(text->s, 1, tree));
		buf_adds(text, more);
		buf_addc(text, '\n');
		free(more);
		status = parse_text(text->s, 0, tree);
	}
	return (status);
}

/*
** A syntax error cannot be recovered from in a script: what is left of a
** string input is skipped, and a shell reading its commands from a file
** descriptor stops there. The terminal simply shows a new prompt.
*/
static void	syntax_error(t_input *in)
{
	sh()->status = 2;
	in->ran = 1;
	if (in->kind == IN_STR)
		in->str = "";
	if (in->kind == IN_FD)
		sh()->unwind = U_EXIT;
}

/*
** Runs the complete command that starts with line (which is consumed).
*/
void	run_text(t_input *in, char *line)
{
	t_buf	text;
	t_node	*tree;
	int	status;

	buf_init(&text);
	buf_adds(&text, line);
	buf_addc(&text, '\n');
	free(line);
	tree = NULL;
	status = read_command(in, &text, &tree);
	if (in->kind == IN_TTY)
		hist_add(text.s);
	if (status == PARSE_ERR)
		syntax_error(in);
	else if (status == PARSE_ABORT)
		sh()->status = 130;
	else if (tree)
	{
		in->ran = 1;
		exec_node(tree, 0);
	}
	node_free(tree);
	buf_free(&text);
}

/*
** Runs every command of an input, until its end or until something
** (exit, return, break, an interrupt) unwinds the shell.
*/
void	run_input(t_input *in)
{
	char	*line;

	line = NULL;
	if (!sh()->unwind)
		line = input_line(in, 0);
	while (line)
	{
		run_text(in, line);
		line = NULL;
		if (!sh()->unwind)
			line = input_line(in, 0);
	}
}
