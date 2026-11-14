#include "sh.h"

/*
** Hands the string over to the caller (never NULL) and resets the buffer.
*/
char	*buf_take(t_buf *b)
{
	char	*s;

	s = b->s;
	if (!s)
		s = xstrdup("");
	buf_init(b);
	return (s);
}

void	buf_free(t_buf *b)
{
	free(b->s);
	buf_init(b);
}

void	buf_addnum(t_buf *b, long n)
{
	char	tmp[32];

	snprintf(tmp, sizeof(tmp), "%ld", n);
	buf_adds(b, tmp);
}

/*
** Writes the whole buffer to fd then frees it. A failed write is reported
** as "42sh: who: write error: ..." and makes the builtin return 1.
*/
int	buf_write(t_buf *b, int fd, const char *who)
{
	size_t	off;
	ssize_t	n;
	int	ret;

	off = 0;
	ret = 0;
	while (off < b->len && ret == 0)
	{
		n = write(fd, b->s + off, b->len - off);
		if (n >= 0)
			off += n;
		else if (errno != EINTR)
		{
			dprintf(STDERR_FILENO, "42sh: %s: write error: %s\n", who,
				strerror(errno));
			ret = 1;
		}
	}
	buf_free(b);
	return (ret);
}
