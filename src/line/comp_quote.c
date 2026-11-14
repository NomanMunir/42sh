#include "sh.h"

/*
** Protects the special characters of a completed file name with
** backslashes, unless the word is already inside quotes.
*/
static char	*comp_quote(char *text, int match_type, char *quote_pointer)
{
	t_buf	out;

	(void)match_type;
	if (quote_pointer && *quote_pointer)
		return (xstrdup(text));
	buf_init(&out);
	while (*text)
	{
		if (ft_strchr(rl_filename_quote_characters, *text))
			buf_addc(&out, '\\');
		buf_addc(&out, *text++);
	}
	return (buf_take(&out));
}

/*
** The reverse operation, applied to the word typed by the user before
** it is compared with file names.
*/
static char	*comp_dequote(char *text, int quote_char)
{
	t_buf	out;

	buf_init(&out);
	while (*text)
	{
		if (*text == '\\' && text[1] && quote_char != '\'')
			text++;
		buf_addc(&out, *text++);
	}
	return (buf_take(&out));
}

/*
** Tells readline that a character preceded by a backslash does not end
** the word being completed.
*/
static int	comp_is_quoted(char *text, int index)
{
	int	slashes;

	slashes = 0;
	while (index - slashes > 0 && text[index - slashes - 1] == '\\')
		slashes++;
	return (slashes % 2);
}

/*
** Makes completion aware of the shell quoting rules. A backslash does
** not break words: it is how a blank is written inside a file name.
*/
void	comp_quote_init(void)
{
	rl_completer_quote_characters = "\"'";
	rl_completer_word_break_characters = " \t\n\"'`$><=;|&{(";
	rl_filename_quote_characters = " \t\n\\\"'<>;|&()#$`?*[";
	rl_filename_quoting_function = comp_quote;
	rl_filename_dequoting_function = comp_dequote;
	rl_char_is_quoted_p = comp_is_quoted;
}
