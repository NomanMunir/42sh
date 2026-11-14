#include "sh.h"

/*
** Is the character at i a field separator (an unescaped IFS character)?
** With blank set, only the IFS white space counts.
*/
static int	read_is_sep(t_buf *line, size_t i, int blank)
{
	const char	*ifs;
	char	c;

	if (i >= line[0].len || line[1].s[i] != '0')
		return (0);
	ifs = var_get("IFS");
	if (!ifs)
		ifs = " \t\n";
	c = line[0].s[i];
	if (blank && c != ' ' && c != '\t' && c != '\n')
		return (0);
	return (ft_strchr(ifs, c) != NULL);
}

/*
** Skips the separator after a field: IFS blanks around at most one
** other IFS character.
*/
static size_t	read_skip_sep(t_buf *line, size_t i)
{
	while (read_is_sep(line, i, 1))
		i++;
	if (read_is_sep(line, i, 0))
		i++;
	while (read_is_sep(line, i, 1))
		i++;
	return (i);
}

/*
** The last name receives everything that is left, without the trailing
** IFS blanks.
*/
static void	read_rest(t_buf *line, size_t i, const char *name)
{
	size_t	end;
	char	*value;

	end = line[0].len;
	while (end > i && read_is_sep(line, end - 1, 1))
		end--;
	value = xstrndup(line[0].s + i, end - i);
	var_set(name, value, 0);
	free(value);
}

/*
** Splits the line into fields and assigns them to the names in order.
** Names left without a field are set to the empty string.
*/
void	read_assign(char **names, t_buf *line)
{
	size_t	i;
	size_t	start;
	char	*value;

	buf_adds(&line[0], "");
	buf_adds(&line[1], "");
	i = 0;
	while (read_is_sep(line, i, 1))
		i++;
	while (names[0] && names[1])
	{
		start = i;
		while (i < line[0].len && !read_is_sep(line, i, 0))
			i++;
		value = xstrndup(line[0].s + start, i - start);
		var_set(names[0], value, 0);
		free(value);
		i = read_skip_sep(line, i);
		names++;
	}
	read_rest(line, i, names[0]);
}
