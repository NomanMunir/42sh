#include "sh.h"

/*
** Called by readline while it waits for a key: a pending ^C drops what
** was typed and ends the line.
*/
static int	line_poll(void)
{
	if (sig_peek() == SIGINT)
	{
		rl_replace_line("", 0);
		rl_done = 1;
	}
	return (0);
}

void	line_init(void)
{
	setlocale(LC_CTYPE, "");
	rl_readline_name = "42sh";
	rl_outstream = stderr;
	rl_event_hook = line_poll;
	rl_attempted_completion_function = line_complete;
	comp_quote_init();
	using_history();
	hist_load();
	line_set_mode(sh()->vi_mode);
}

void	line_cleanup(void)
{
	hist_save();
	clear_history();
}

/*
** Switches between the vi and the readline (emacs) editing modes.
*/
void	line_set_mode(int vi)
{
	if (vi)
		rl_variable_bind("editing-mode", "vi");
	else
		rl_variable_bind("editing-mode", "emacs");
	line_bind_keys();
}

/*
** Reads one line with full line editing. Returns NULL at end of input
** (^D on an empty line). After ^C the line is empty and the interrupt
** is left pending for the caller.
*/
char	*line_read(int cont)
{
	char	*prompt;
	char	*line;

	prompt = prompt_build(cont);
	line = readline(prompt);
	free(prompt);
	if (sig_peek() != SIGINT)
		return (line);
	free(line);
	sh()->status = 128 + SIGINT;
	return (xstrdup(""));
}
