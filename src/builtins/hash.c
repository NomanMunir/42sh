#include "sh.h"

/*
** Lists the remembered commands with the number of times each was used.
*/
static int	hash_list(void)
{
	t_buf	out;
	t_hashent	*entry;
	char	**keys;
	int	i;

	keys = map_keys(&sh()->hash);
	buf_init(&out);
	if (!keys[0])
		buf_adds(&out, "hash: hash table empty\n");
	else
		buf_adds(&out, "hits\tcommand\n");
	i = 0;
	while (keys[i])
	{
		entry = map_get(&sh()->hash, keys[i++]);
		buf_addnum(&out, entry->hits);
		buf_addc(&out, '\t');
		buf_adds(&out, entry->path);
		buf_addc(&out, '\n');
	}
	free(keys);
	return (buf_write(&out, STDOUT_FILENO, "hash"));
}

/*
** Locates a command in PATH and remembers where it is.
*/
static int	hash_add(const char *name)
{
	t_hashent	*entry;
	char	*path;

	if (ft_strchr(name, '/'))
		return (0);
	path = path_search(name);
	if (!path)
	{
		dprintf(STDERR_FILENO, "42sh: hash: %s: not found\n", name);
		return (1);
	}
	entry = xcalloc(1, sizeof(t_hashent));
	entry->path = path;
	map_set(&sh()->hash, name, entry);
	return (0);
}

/*
** hash [-r] [utility...]
*/
int	bi_hash(char **argv)
{
	int	flags;
	int	status;
	int	i;

	i = opt_parse(argv, "r", &flags);
	if (i < 0)
		return (2);
	if (flags)
		map_clear(&sh()->hash);
	else if (!argv[i])
		return (hash_list());
	status = 0;
	while (argv[i])
		status |= hash_add(argv[i++]);
	return (status);
}
