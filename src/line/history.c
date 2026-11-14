#include "sh.h"

/*
** Number of commands kept, from $HISTSIZE.
*/
static int	hist_size(void)
{
	char	*value;
	long	size;

	value = var_get("HISTSIZE");
	if (!value || !parse_long(value, &size) || size < 0 || size > 1000000)
		return (HIST_DEFAULT_SIZE);
	return ((int)size);
}

/*
** Reads the history of the previous sessions from $HISTFILE.
*/
void	hist_load(void)
{
	char	*file;

	file = var_get("HISTFILE");
	if (file && *file)
		read_history(file);
	stifle_history(hist_size());
}

void	hist_save(void)
{
	char	*file;

	file = var_get("HISTFILE");
	if (!file || !*file)
		return ;
	stifle_history(hist_size());
	write_history(file);
}

/*
** Records a command, without its final newline. Blank lines are not
** worth remembering.
*/
void	hist_add(const char *text)
{
	char	*entry;
	size_t	len;

	len = ft_strlen(text);
	while (len > 0 && text[len - 1] == '\n')
		len--;
	if (strspn(text, " \t\n") >= len)
		return ;
	entry = xstrndup(text, len);
	add_history(entry);
	free(entry);
}
