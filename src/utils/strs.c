#include "sh.h"

int	strs_len(char **v)
{
	int	n;

	n = 0;
	while (v && v[n])
		n++;
	return (n);
}

static int	strs_compare(const void *a, const void *b)
{
	return (ft_strcmp(*(char *const *)a, *(char *const *)b));
}

void	strs_sort(char **v, int n)
{
	if (v && n > 1)
		qsort(v, n, sizeof(char *), strs_compare);
}

char	**strs_dup(char **v)
{
	t_strv	copy;
	int	i;

	strv_init(&copy);
	i = 0;
	while (v && v[i])
		strv_push(&copy, xstrdup(v[i++]));
	return (strv_take(&copy));
}

char	*strs_join(char **v, const char *sep)
{
	t_buf	buf;
	int	i;

	buf_init(&buf);
	i = 0;
	while (v && v[i])
	{
		if (i > 0)
			buf_adds(&buf, sep);
		buf_adds(&buf, v[i++]);
	}
	return (buf_take(&buf));
}
