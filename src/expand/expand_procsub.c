#include "sh.h"

/*
** Closes the pipe ends kept open for the /dev/fd/N names handed out by
** process substitutions, once the command that used them is over.
*/
void	procsub_close(void)
{
	t_shell	*s;

	s = sh();
	while (s->nprocsub > 0)
		close(s->procsub[--s->nprocsub]);
}

static void	procsub_child(int *fds, const char *cmd, int reading)
{
	sh_enter_child(0);
	if (reading)
		move_fd(fds[1], STDOUT_FILENO);
	else
		move_fd(fds[0], STDIN_FILENO);
	close(fds[!reading]);
	exit(sh_run_string(cmd));
}

/*
** Starts cmd connected to a pipe; returns our end of the pipe.
** reading: <(cmd), we read what the command writes.
*/
static int	procsub_spawn(const char *cmd, int reading)
{
	int	fds[2];
	pid_t	pid;

	if (sh()->nprocsub >= PROCSUB_MAX || pipe(fds) < 0)
		return (-1);
	pid = fork();
	if (pid == 0)
		procsub_child(fds, cmd, reading);
	close(fds[reading]);
	if (pid < 0)
	{
		close(fds[!reading]);
		return (-1);
	}
	sh()->procsub[sh()->nprocsub++] = fds[!reading];
	return (fds[!reading]);
}

/*
** <(command) and >(command) expand to the name of a file descriptor
** connected to the command.
*/
size_t	exp_procsub(t_exp *e, const char *s)
{
	char	path[32];
	char	*cmd;
	int	len;
	int	fd;

	len = scan_group(s + 1);
	if (len < 0)
	{
		exp_put(e, s[0], M_LIT);
		return (1);
	}
	cmd = xstrndup(s + 2, len - 2);
	fd = procsub_spawn(cmd, s[0] == '<');
	free(cmd);
	if (fd < 0)
	{
		dprintf(STDERR_FILENO, "42sh: cannot make process substitution\n");
		e->err = 1;
		return (len + 1);
	}
	snprintf(path, sizeof(path), "/dev/fd/%d", fd);
	exp_puts(e, path, M_QUOTED);
	return (len + 1);
}
