#include "sh.h"

char	*xstrndup(const char *s, size_t n)
{
	char	*dup;

	dup = xmalloc(n + 1);
	ft_memcpy(dup, s, n);
	dup[n] = '\0';
	return (dup);
}

char	*xstrdup(const char *s)
{
	return (xstrndup(s, ft_strlen(s)));
}

char	*xjoin3(const char *a, const char *b, const char *c)
{
	t_buf	buf;

	buf_init(&buf);
	buf_adds(&buf, a);
	buf_adds(&buf, b);
	buf_adds(&buf, c);
	return (buf_take(&buf));
}

char	*xjoin(const char *a, const char *b)
{
	return (xjoin3(a, b, NULL));
}

char	*xitoa(long n)
{
	char	tmp[32];

	snprintf(tmp, sizeof(tmp), "%ld", n);
	return (xstrdup(tmp));
}
