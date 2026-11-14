#include "sh.h"

/*
** First line of dir/.git/HEAD, NULL if dir is not the top of a
** repository.
*/
static char	*git_head(const char *dir)
{
	char	*path;
	char	*text;
	int	fd;

	path = xjoin(dir, "/.git/HEAD");
	fd = open(path, O_RDONLY);
	free(path);
	if (fd < 0)
		return (NULL);
	text = file_read_all(fd);
	close(fd);
	text[strcspn(text, "\n")] = '\0';
	return (text);
}

/*
** HEAD of the repository the current directory belongs to: looked for
** in the directory itself, then in each of its parents.
*/
static char	*git_find_head(void)
{
	char	*dir;
	char	*head;
	char	*slash;

	if (!var_get("PWD"))
		return (NULL);
	dir = xstrdup(var_get("PWD"));
	head = git_head(dir);
	slash = ft_strrchr(dir, '/');
	while (!head && slash)
	{
		*slash = '\0';
		head = git_head(dir);
		slash = ft_strrchr(dir, '/');
	}
	free(dir);
	return (head);
}

/*
** Name of the git branch the current directory is on (or the start of
** the commit id when no branch is checked out). NULL outside of a
** repository.
*/
char	*prompt_git_branch(void)
{
	char	*head;
	char	*branch;

	head = git_find_head();
	if (!head)
		return (NULL);
	branch = NULL;
	if (ft_strncmp(head, "ref: refs/heads/", 16) == 0)
		branch = xstrdup(head + 16);
	else if (strspn(head, "0123456789abcdef") >= 7)
		branch = xstrndup(head, 7);
	free(head);
	return (branch);
}

/*
** \g: " (branch)" inside a git repository, nothing elsewhere.
*/
void	prompt_put_branch(t_buf *out)
{
	char	*branch;

	branch = prompt_git_branch();
	if (!branch)
		return ;
	buf_adds(out, " (");
	prompt_put_safe(out, branch);
	buf_addc(out, ')');
	free(branch);
}
