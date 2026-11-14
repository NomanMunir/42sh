#include "sh.h"

int	hist_count(void)
{
	return (history_length);
}

/*
** Command at position pos (0 is the oldest one kept), NULL if there is
** no such position.
*/
const char	*hist_line(int pos)
{
	HIST_ENTRY	*entry;

	if (pos < 0 || pos >= history_length)
		return (NULL);
	entry = history_get(history_base + pos);
	if (!entry)
		return (NULL);
	return (entry->line);
}

/*
** History number of the command at position pos, as shown by fc -l.
*/
int	hist_number(int pos)
{
	return (history_base + pos);
}

void	hist_drop_last(void)
{
	HIST_ENTRY	*entry;

	if (history_length < 1)
		return ;
	entry = remove_history(history_length - 1);
	if (entry)
		free_history_entry(entry);
}

void	hist_clear(void)
{
	clear_history();
}
