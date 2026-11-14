#include "sh.h"

char	*file_read_all(int fd)
{
	t_buf	buf;
	char	chunk[4096];
	ssize_t	n;

	buf_init(&buf);
	n = 1;
	while (n > 0 || (n < 0 && errno == EINTR))
	{
		n = read(fd, chunk, sizeof(chunk));
		if (n > 0)
			buf_addn(&buf, chunk, n);
	}
	return (buf_take(&buf));
}

static void	cmdsub_child(int *fds, const char *cmd)
{
	sh_enter_child(0);
	close(fds[0]);
	move_fd(fds[1], STDOUT_FILENO);
	exit(sh_run_string(cmd));
}

static int	wait_child(pid_t pid)
{
	int	wstatus;

	wstatus = 0;
	while (waitpid(pid, &wstatus, 0) < 0)
	{
		if (errno != EINTR)
			return (1);
	}
	return (status_code(wstatus));
}

/*
** $(command): runs the command in a child shell and returns its output
** without the trailing newlines. NULL if the child could not be created.
*/
char	*cmdsub_run(const char *cmd)
{
	int	fds[2];
	pid_t	pid;
	char	*out;
	size_t	len;

	if (pipe(fds) < 0)
		return (NULL);
	pid = fork();
	if (pid == 0)
		cmdsub_child(fds, cmd);
	close(fds[1]);
	if (pid < 0)
	{
		dprintf(STDERR_FILENO, "42sh: fork: %s\n", strerror(errno));
		close(fds[0]);
		return (NULL);
	}
	out = file_read_all(fds[0]);
	close(fds[0]);
	sh()->subst_status = wait_child(pid);
	len = ft_strlen(out);
	while (len > 0 && out[len - 1] == '\n')
		out[--len] = '\0';
	return (out);
}
