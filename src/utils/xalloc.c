#include "sh.h"

/*
** Allocation wrappers: running out of memory is fatal for a shell, so the
** callers never have to deal with NULL.
*/
static void	out_of_memory(void)
{
	write(STDERR_FILENO, "42sh: out of memory\n", 20);
	exit(2);
}

void	*xmalloc(size_t size)
{
	void	*ptr;

	if (size == 0)
		size = 1;
	ptr = malloc(size);
	if (!ptr)
		out_of_memory();
	return (ptr);
}

void	*xcalloc(size_t count, size_t size)
{
	void	*ptr;

	ptr = xmalloc(count * size);
	ft_bzero(ptr, count * size);
	return (ptr);
}

void	*xrealloc(void *ptr, size_t size)
{
	if (size == 0)
		size = 1;
	ptr = realloc(ptr, size);
	if (!ptr)
		out_of_memory();
	return (ptr);
}
