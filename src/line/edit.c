#include "sh.h"

/*
** The editor to run: the one asked for, else $FCEDIT, $VISUAL, $EDITOR,
** and vi as a last resort.
*/
static const char	*editor_command(const char *editor)
{
	static const char	*vars[] = {"FCEDIT", "VISUAL", "EDITOR", NULL};
	int	i;

	i = 0;
	while (!(editor && *editor) && vars[i])
		editor = var_get(vars[i++]);
	if (!editor || !*editor)
		return ("vi");
	return (editor);
}

/*
** Runs the editor on path as an ordinary foreground command.
*/
static int	editor_run(const char *editor, const char *path)
{
	char	*quoted;
	char	*command;
	int	status;

	quoted = sh_quote(path);
	command = xjoin3(editor_command(editor), " ", quoted);
	status = sh_run_string(command);
	free(command);
	free(quoted);
	return (status);
}

static char	*edit_result(const char *path)
{
	char	*text;
	size_t	len;
	int	fd;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (NULL);
	text = file_read_all(fd);
	close(fd);
	len = ft_strlen(text);
	while (len > 0 && text[len - 1] == '\n')
		text[--len] = '\0';
	return (text);
}

/*
** Lets the user edit text in an editor, through a temporary file.
** Returns the edited text, NULL if the editor could not do its job.
*/
char	*edit_text(const char *text, const char *editor)
{
	char	path[32];
	char	*result;
	t_buf	content;
	int	fd;

	ft_strlcpy(path, "/tmp/42sh-edit-XXXXXX", sizeof(path));
	fd = mkstemp(path);
	if (fd < 0)
	{
		dprintf(STDERR_FILENO, "42sh: cannot create a temporary file: %s\n",
			strerror(errno));
		return (NULL);
	}
	buf_init(&content);
	buf_adds(&content, text);
	if (content.len > 0 && content.s[content.len - 1] != '\n')
		buf_addc(&content, '\n');
	result = NULL;
	if (buf_write(&content, fd, "edit") == 0 && close(fd) == 0
		&& editor_run(editor, path) == 0)
		result = edit_result(path);
	unlink(path);
	return (result);
}
