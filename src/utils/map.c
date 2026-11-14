#include "sh.h"

/*
** FNV-1a hash.
*/
size_t	map_hash(const char *key)
{
	size_t	hash;

	hash = 1469598103934665603UL;
	while (*key)
	{
		hash ^= (unsigned char)*key++;
		hash *= 1099511628211UL;
	}
	return (hash);
}

void	map_init(t_map *m, t_delfn del)
{
	m->size = 64;
	m->count = 0;
	m->del = del;
	m->tab = xcalloc(m->size, sizeof(t_ment *));
}

static t_ment	*map_find(t_map *m, const char *key)
{
	t_ment	*ent;

	if (!m->tab)
		return (NULL);
	ent = m->tab[map_hash(key) % m->size];
	while (ent && ft_strcmp(ent->key, key) != 0)
		ent = ent->next;
	return (ent);
}

void	*map_get(t_map *m, const char *key)
{
	t_ment	*ent;

	ent = map_find(m, key);
	if (!ent)
		return (NULL);
	return (ent->val);
}

/*
** Inserts or replaces; a replaced value is released with the map's delfn.
*/
void	map_set(t_map *m, const char *key, void *val)
{
	t_ment	*ent;
	size_t	slot;

	ent = map_find(m, key);
	if (ent)
	{
		if (m->del && ent->val != val)
			m->del(ent->val);
		ent->val = val;
		return ;
	}
	if (m->count >= m->size)
		map_grow(m);
	slot = map_hash(key) % m->size;
	ent = xmalloc(sizeof(t_ment));
	ent->key = xstrdup(key);
	ent->val = val;
	ent->next = m->tab[slot];
	m->tab[slot] = ent;
	m->count++;
}
