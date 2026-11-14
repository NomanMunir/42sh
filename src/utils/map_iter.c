#include "sh.h"

int	map_del(t_map *m, const char *key)
{
	t_ment	**link;
	t_ment	*ent;

	if (!m->tab)
		return (0);
	link = &m->tab[map_hash(key) % m->size];
	while (*link && ft_strcmp((*link)->key, key) != 0)
		link = &(*link)->next;
	ent = *link;
	if (!ent)
		return (0);
	*link = ent->next;
	if (m->del)
		m->del(ent->val);
	free(ent->key);
	free(ent);
	m->count--;
	return (1);
}

/*
** Doubles the number of buckets and redistributes the entries.
*/
void	map_grow(t_map *m)
{
	t_ment	**old;
	t_ment	*ent;
	t_ment	*next;
	size_t	i;
	size_t	slot;

	old = m->tab;
	m->tab = xcalloc(m->size * 2, sizeof(t_ment *));
	i = 0;
	while (i < m->size)
	{
		ent = old[i++];
		while (ent)
		{
			next = ent->next;
			slot = map_hash(ent->key) % (m->size * 2);
			ent->next = m->tab[slot];
			m->tab[slot] = ent;
			ent = next;
		}
	}
	m->size *= 2;
	free(old);
}

void	map_clear(t_map *m)
{
	t_ment	*ent;
	t_ment	*next;
	size_t	i;

	i = 0;
	while (m->tab && i < m->size)
	{
		ent = m->tab[i];
		while (ent)
		{
			next = ent->next;
			if (m->del)
				m->del(ent->val);
			free(ent->key);
			free(ent);
			ent = next;
		}
		m->tab[i++] = NULL;
	}
	m->count = 0;
}

void	map_free(t_map *m)
{
	map_clear(m);
	free(m->tab);
	m->tab = NULL;
	m->size = 0;
}

/*
** Sorted, NULL-terminated array of the keys. The strings still belong to
** the map: the caller frees the array only.
*/
char	**map_keys(t_map *m)
{
	char	**keys;
	t_ment	*ent;
	size_t	i;
	int	n;

	keys = xcalloc(m->count + 1, sizeof(char *));
	n = 0;
	i = 0;
	while (m->tab && i < m->size)
	{
		ent = m->tab[i++];
		while (ent)
		{
			keys[n++] = ent->key;
			ent = ent->next;
		}
	}
	strs_sort(keys, n);
	return (keys);
}
