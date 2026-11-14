#include "sh.h"

/*
** What the arguments of a command are, for the commands whose arguments
** are not file names.
*/
int	comp_for_command(const char *word)
{
	static const char	*names[] = {"cd", "unalias", "alias", "unset",
		"export", "read", "type", "hash", "exec", "fg", "bg", "jobs", NULL};
	static const int	kinds[] = {C_DIR, C_ALIAS, C_ALIAS, C_VARNAME,
		C_VARNAME, C_VARNAME, C_CMD, C_CMD, C_CMD, C_JOB, C_JOB, C_JOB};
	int	i;

	i = 0;
	while (names[i] && !str_eq(names[i], word))
		i++;
	if (!names[i])
		return (C_FILE);
	return (kinds[i]);
}

/*
** A word after which a new command starts ("if", "then", "!"...).
*/
static int	comp_is_prefix_word(const char *word)
{
	return (str_eq(word, "!") || str_eq(word, "{") || str_eq(word, "if")
		|| str_eq(word, "then") || str_eq(word, "else")
		|| str_eq(word, "elif") || str_eq(word, "do")
		|| str_eq(word, "while") || str_eq(word, "until"));
}

/*
** Context of an argument: cmd is the text of the current command up to
** the word being completed (len characters).
*/
static int	comp_argument(const char *cmd, int len)
{
	char	*word;
	int	used;
	int	ctx;

	used = (int)strcspn(cmd, " \t");
	if (used >= len)
		return (C_CMD);
	word = xstrndup(cmd, used);
	used += (int)strspn(cmd + used, " \t");
	if (comp_is_prefix_word(word))
		ctx = comp_argument(cmd + used, len - used);
	else
		ctx = comp_for_command(word);
	if (str_eq(word, "set") && len >= 3 && (cmd[len - 1] == ' ')
		&& cmd[len - 2] == 'o' && (cmd[len - 3] == '-' || cmd[len - 3] == '+'))
		ctx = C_SETOPT;
	if ((str_eq(word, "kill") || str_eq(word, "wait")) && cmd[len] == '%')
		ctx = C_JOB;
	free(word);
	return (ctx);
}

/*
** Works out what the word starting at position start of the line is:
** a variable name, a command name or an argument of some kind.
*/
int	comp_context(int start)
{
	const char	*line;
	int	i;

	line = rl_line_buffer;
	if (start >= 2 && line[start - 1] == '{' && line[start - 2] == '$')
		return (C_VARBRACE);
	if (start >= 1 && line[start - 1] == '$')
		return (C_VAR);
	i = start;
	while (i > 0 && !ft_strchr(";|&(\n`", line[i - 1]))
		i--;
	while (i < start && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (i >= start)
		return (C_CMD);
	return (comp_argument(line + i, start - i));
}

/*
** Completion entry point called by readline for the word text, found
** at [start, end) in the line. Returning NULL lets readline complete a
** file name.
*/
char	**line_complete(const char *text, int start, int end)
{
	int	ctx;

	(void)end;
	rl_completion_append_character = ' ';
	ctx = comp_context(start);
	if (ctx == C_FILE || (ctx == C_CMD && ft_strchr(text, '/')))
		return (NULL);
	if (ctx == C_DIR)
	{
		rl_filename_completion_desired = 1;
		rl_attempted_completion_over = 1;
		return (rl_completion_matches(text, comp_gen_dir));
	}
	comp_collect(ctx, text);
	if (ctx == C_CMD && sh()->comp.len == 0)
		return (NULL);
	if (ctx == C_VARBRACE)
		rl_completion_append_character = '}';
	rl_attempted_completion_over = 1;
	return (rl_completion_matches(text, comp_gen_list));
}
