#include "sh.h"

void	move_fd(int from, int to)
{
	if (from < 0 || from == to)
		return ;
	dup2(from, to);
	close(from);
}

/*
** Remembers what fd currently refers to (a private copy above 10, or -1
** if it is closed) before a redirection replaces it.
*/
void	fd_save(t_fdsave **save, int fd)
{
	t_fdsave	*entry;

	if (!save)
		return ;
	entry = xmalloc(sizeof(t_fdsave));
	entry->fd = fd;
	entry->copy = fcntl(fd, F_DUPFD_CLOEXEC, 10);
	entry->next = *save;
	*save = entry;
}

/*
** Undoes the redirections, most recent first.
*/
void	redir_restore(t_fdsave **save)
{
	t_fdsave	*entry;

	while (*save)
	{
		entry = *save;
		*save = entry->next;
		if (entry->copy >= 0)
		{
			dup2(entry->copy, entry->fd);
			close(entry->copy);
		}
		else
			close(entry->fd);
		free(entry);
	}
}

/*
** Makes the redirections permanent ("exec > file").
*/
void	redir_discard(t_fdsave **save)
{
	t_fdsave	*entry;

	while (*save)
	{
		entry = *save;
		*save = entry->next;
		if (entry->copy >= 0)
			close(entry->copy);
		free(entry);
	}
}
