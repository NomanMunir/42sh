#include "sh.h"

/*
** A small body fits in a pipe without blocking.
*/
static int	heredoc_pipe(const char *body, size_t len)
{
	int	fds[2];

	if (pipe(fds) < 0)
		return (-1);
	if (write(fds[1], body, len) < 0)
	{
		close(fds[0]);
		fds[0] = -1;
	}
	close(fds[1]);
	return (fds[0]);
}

/*
** A large one goes through an anonymous temporary file.
*/
static int	heredoc_file(const char *body, size_t len)
{
	char	path[32];
	size_t	done;
	ssize_t	n;
	int	fd;

	ft_strlcpy(path, "/tmp/42sh-heredoc-XXXXXX", sizeof(path));
	fd = mkstemp(path);
	if (fd < 0)
		return (-1);
	unlink(path);
	done = 0;
	n = 0;
	while (done < len && n >= 0)
	{
		n = write(fd, body + done, len - done);
		if (n > 0)
			done += n;
	}
	if (n < 0 || lseek(fd, 0, SEEK_SET) < 0)
	{
		close(fd);
		return (-1);
	}
	return (fd);
}

/*
** Returns a descriptor from which the here-document can be read. The body
** is expanded unless its delimiter was quoted.
*/
int	heredoc_open(t_redir *r)
{
	char	*body;
	size_t	len;
	int	fd;

	if (!r->body)
		body = xstrdup("");
	else if (r->quoted)
		body = xstrdup(r->body);
	else
		body = expand_single(r->body, EXP_HEREDOC);
	if (!body)
		return (-1);
	len = ft_strlen(body);
	if (len <= 4096)
		fd = heredoc_pipe(body, len);
	else
		fd = heredoc_file(body, len);
	if (fd < 0)
		dprintf(STDERR_FILENO, "42sh: here-document: %s\n", strerror(errno));
	free(body);
	return (fd);
}
