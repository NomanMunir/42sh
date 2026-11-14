#include "sh.h"

void	strv_init(t_strv *sv)
{
	sv->v = NULL;
	sv->len = 0;
	sv->cap = 0;
}

/*
** Appends s (ownership is transferred); the array stays NULL-terminated.
*/
void	strv_push(t_strv *sv, char *s)
{
	if (sv->len + 2 > sv->cap)
	{
		if (sv->cap == 0)
			sv->cap = 8;
		else
			sv->cap *= 2;
		sv->v = xrealloc(sv->v, sv->cap * sizeof(char *));
	}
	sv->v[sv->len++] = s;
	sv->v[sv->len] = NULL;
}

char	**strv_take(t_strv *sv)
{
	char	**v;

	v = sv->v;
	if (!v)
		v = xcalloc(1, sizeof(char *));
	strv_init(sv);
	return (v);
}

void	strv_free(t_strv *sv)
{
	free_strs(sv->v);
	strv_init(sv);
}

void	free_strs(char **v)
{
	int	i;

	if (!v)
		return ;
	i = 0;
	while (v[i])
		free(v[i++]);
	free(v);
}
