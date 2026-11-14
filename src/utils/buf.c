#include "sh.h"

void	buf_init(t_buf *b)
{
	b->s = NULL;
	b->len = 0;
	b->cap = 0;
}

static void	buf_reserve(t_buf *b, size_t extra)
{
	size_t	need;

	need = b->len + extra + 1;
	if (need <= b->cap)
		return ;
	if (b->cap == 0)
		b->cap = 32;
	while (b->cap < need)
		b->cap *= 2;
	b->s = xrealloc(b->s, b->cap);
}

void	buf_addc(t_buf *b, char c)
{
	buf_reserve(b, 1);
	b->s[b->len++] = c;
	b->s[b->len] = '\0';
}

void	buf_addn(t_buf *b, const char *s, size_t n)
{
	buf_reserve(b, n);
	ft_memcpy(b->s + b->len, s, n);
	b->len += n;
	b->s[b->len] = '\0';
}

void	buf_adds(t_buf *b, const char *s)
{
	if (s)
		buf_addn(b, s, ft_strlen(s));
}
