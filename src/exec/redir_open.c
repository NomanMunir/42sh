#include "sh.h"

int	redir_open(t_tok op, const char *path)
{
	int	flags;
	int	fd;

	flags = O_RDONLY;
	if (op == T_GREAT || op == T_CLOBBER || op == T_GREATAND)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else if (op == T_DGREAT)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else if (op == T_LESSGREAT)
		flags = O_RDWR | O_CREAT;
	fd = open(path, flags, 0666);
	if (fd < 0)
		dprintf(STDERR_FILENO, "42sh: %s: %s\n", path, strerror(errno));
	return (fd);
}

static int	is_number(const char *s)
{
	int	i;

	i = 0;
	while (ft_isdigit(s[i]))
		i++;
	return (i > 0 && i < 10 && !s[i]);
}

/*
** ">&word" with a word that is not a descriptor is the csh/bash shortcut
** for "> word 2>&1" (only without an explicit descriptor on the left).
*/
int	redir_is_both(t_redir *r)
{
	return (r->op == T_GREATAND && r->fd < 0 && !is_number(r->word)
		&& !str_eq(r->word, "-") && r->word[0] != '$');
}

/*
** Source of "n<&word" / "n>&word": a copy of descriptor word, -2 to close
** the target when word is "-".
*/
int	redir_dup_source(t_redir *r, const char *word)
{
	int	fd;

	if (str_eq(word, "-"))
		return (-2);
	if (!is_number(word))
	{
		if (redir_is_both(r))
			return (redir_open(T_GREAT, word));
		dprintf(STDERR_FILENO, "42sh: %s: ambiguous redirect\n", word);
		return (-1);
	}
	fd = fcntl(ft_atoi(word), F_DUPFD, 10);
	if (fd < 0)
		dprintf(STDERR_FILENO, "42sh: %s: %s\n", word, strerror(errno));
	return (fd);
}
