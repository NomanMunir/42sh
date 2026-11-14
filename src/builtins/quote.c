#include "sh.h"

/*
** Returns value written so that the shell reads it back as one word:
** unchanged when it only holds safe characters, otherwise between single
** quotes (a quote inside becomes '\'').
*/
char	*sh_quote(const char *value)
{
	t_buf	out;
	size_t	i;

	i = 0;
	while (ft_isalnum(value[i]) || (value[i] && ft_strchr("_/.:,+@%-",
				value[i])))
		i++;
	if (i > 0 && !value[i])
		return (xstrdup(value));
	buf_init(&out);
	buf_addc(&out, '\'');
	while (*value)
	{
		if (*value == '\'')
			buf_adds(&out, "'\\''");
		else
			buf_addc(&out, *value);
		value++;
	}
	buf_addc(&out, '\'');
	return (buf_take(&out));
}
