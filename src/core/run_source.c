#include "sh.h"

/*
** Runs the commands of a string (-c, eval, substitutions, scripts) and
** returns the status: the one of the last command, 0 if there was none.
*/
int	sh_run_string(const char *str)
{
	t_shell	*s;
	t_input	in;

	s = sh();
	if (s->nest >= NEST_MAX)
	{
		dprintf(STDERR_FILENO, "42sh: maximum nesting level exceeded\n");
		s->status = 1;
		return (1);
	}
	ft_bzero(&in, sizeof(in));
	in.kind = IN_STR;
	in.fd = -1;
	in.str = str;
	s->nest++;
	run_input(&in);
	s->nest--;
	if (!in.ran && !s->unwind)
		s->status = 0;
	return (s->status);
}

static int	file_error(const char *path, int error, int toplevel)
{
	dprintf(STDERR_FILENO, "42sh: %s: %s\n", path, strerror(error));
	if (!toplevel)
		return (1);
	if (error == ENOENT)
		return (127);
	return (126);
}

/*
** Runs a script file in the current shell. toplevel is set for the
** script named on the command line, whose failures use the statuses of
** a command that cannot be executed (126, 127).
*/
int	sh_run_file(const char *path, int toplevel)
{
	struct stat	info;
	char	*text;
	int	fd;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (file_error(path, errno, toplevel));
	if (fstat(fd, &info) == 0 && S_ISDIR(info.st_mode))
	{
		close(fd);
		return (file_error(path, EISDIR, toplevel));
	}
	text = file_read_all(fd);
	close(fd);
	sh_run_string(text);
	free(text);
	return (sh()->status);
}

/*
** End of input at the prompt (^D). Returns 1 when the shell must leave;
** stopped jobs get one warning first, like with the exit builtin.
*/
static int	prompt_eof(void)
{
	t_shell	*s;

	s = sh();
	write(STDERR_FILENO, "exit\n", 5);
	if (!jobs_stopped() || s->exit_warned)
		return (1);
	dprintf(STDERR_FILENO, "There are stopped jobs.\n");
	s->exit_warned = 2;
	return (0);
}

/*
** Main loop of the shell. An interactive session starts every command
** from a clean state: pending interrupts are dropped and the jobs whose
** state changed are reported before the prompt. A ^C that stopped the
** shell itself (a loop of builtins, read...) still owes its newline.
*/
void	sh_loop(t_input *in)
{
	t_shell	*s;
	char	*line;

	s = sh();
	if (!s->interactive)
		run_input(in);
	while (s->interactive && s->unwind != U_EXIT)
	{
		s->unwind = U_NONE;
		sig_take();
		jobs_notify();
		line = input_line(in, 0);
		sig_take();
		if (line)
			run_text(in, line);
		else if (prompt_eof())
			break ;
		if (sig_take() == SIGINT)
		{
			write(STDERR_FILENO, "\n", 1);
			s->status = 128 + SIGINT;
		}
		if (s->exit_warned > 0)
			s->exit_warned--;
	}
}
