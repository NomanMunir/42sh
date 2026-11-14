#include "sh.h"

/*
** Opens (or duplicates) what the redirection points to. Returns a file
** descriptor owned by the caller, -1 on error, -2 if the target descriptor
** must simply be closed ("<&-" and ">&-").
*/
static int	redir_source(t_redir *r)
{
	char	*word;
	int	source;

	if (r->op == T_DLESS || r->op == T_DLESSDASH)
		return (heredoc_open(r));
	word = expand_single(r->word, EXP_TILDE | EXP_PROCSUB);
	if (!word)
		return (-1);
	if (r->op == T_LESSAND || r->op == T_GREATAND)
		source = redir_dup_source(r, word);
	else
		source = redir_open(r->op, word);
	free(word);
	return (source);
}

static int	redir_target_fd(t_redir *r)
{
	if (r->fd >= 0)
		return (r->fd);
	if (r->op == T_GREAT || r->op == T_DGREAT || r->op == T_GREATAND
		|| r->op == T_CLOBBER)
		return (STDOUT_FILENO);
	return (STDIN_FILENO);
}

static int	redir_dup(int source, int target)
{
	if (dup2(source, target) >= 0)
		return (0);
	dprintf(STDERR_FILENO, "42sh: %d: %s\n", target, strerror(errno));
	close(source);
	return (-1);
}

static int	redir_one(t_redir *r, t_fdsave **save)
{
	int	target;
	int	source;

	target = redir_target_fd(r);
	fd_save(save, target);
	source = redir_source(r);
	if (source == -1)
		return (-1);
	if (source == -2)
	{
		close(target);
		return (0);
	}
	if (redir_dup(source, target) < 0)
		return (-1);
	if (redir_is_both(r))
	{
		fd_save(save, STDERR_FILENO);
		dup2(source, STDERR_FILENO);
	}
	if (source != target)
		close(source);
	return (0);
}

/*
** Applies a list of redirections from left to right. When save is not
** NULL, the descriptors that get replaced are saved so that
** redir_restore can put them back (builtins, groups...).
*/
int	redir_apply(t_redir *r, t_fdsave **save)
{
	while (r)
	{
		if (redir_one(r, save) < 0)
			return (-1);
		r = r->next;
	}
	return (0);
}
