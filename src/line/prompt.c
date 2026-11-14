#include "sh.h"

/*
** Appends text that comes from outside (directory, user, host, branch
** names) with its control characters replaced, so that it can never
** send an escape sequence to the terminal.
*/
void	prompt_put_safe(t_buf *out, const char *s)
{
	while (s && *s)
	{
		if ((unsigned char)*s < 32 || *s == 127)
			buf_addc(out, '?');
		else
			buf_addc(out, *s);
		s++;
	}
}

/*
** \w: the current directory with $HOME shown as ~. \W: its last
** component only.
*/
static void	prompt_cwd(t_buf *out, int basename)
{
	const char	*pwd;
	const char	*home;
	const char	*base;
	size_t	len;

	pwd = var_get("PWD");
	home = var_get("HOME");
	if (!pwd)
		pwd = "?";
	len = 0;
	if (home && home[0] == '/' && home[1])
		len = ft_strlen(home);
	base = ft_strrchr(pwd, '/');
	if (len && ft_strncmp(pwd, home, len) == 0
		&& (!pwd[len] || (pwd[len] == '/' && !basename)))
	{
		buf_addc(out, '~');
		pwd += len;
	}
	else if (basename && base && base[1])
		pwd = base + 1;
	prompt_put_safe(out, pwd);
}

/*
** Escapes that depend on the state of the shell:
** \u user  \h host  \w \W directory  \$ prompt character  \s shell name
** \g git branch  \c colour of the last status (green or red)
*/
static void	prompt_dynamic(t_buf *out, char c)
{
	char	host[256];

	host[0] = '\0';
	if (c == 'u')
		prompt_put_safe(out, var_get("USER"));
	else if (c == 'w' || c == 'W')
		prompt_cwd(out, c == 'W');
	else if (c == 's')
		buf_adds(out, "42sh");
	else if (c == '$' && geteuid() == 0)
		buf_addc(out, '#');
	else if (c == '$')
		buf_addc(out, '$');
	else if (c == 'c' && sh()->status == 0)
		buf_adds(out, "\001\033[1;32m\002");
	else if (c == 'c')
		buf_adds(out, "\001\033[1;31m\002");
	else if (c == 'h' && gethostname(host, sizeof(host) - 1) == 0)
	{
		host[strcspn(host, ".")] = '\0';
		prompt_put_safe(out, host);
	}
}

/*
** Appends the meaning of the prompt escape \c. \[ and \] surround
** sequences that do not move the cursor (colours). An unknown escape is
** kept as it is.
*/
void	prompt_escape(t_buf *out, char c)
{
	static const char	*names = "e[]n\\a";
	static const char	*values = "\033\001\002\n\\\a";
	const char	*found;

	found = NULL;
	if (c)
		found = ft_strchr(names, c);
	if (found)
		buf_addc(out, values[found - names]);
	else if (c == 'g')
		prompt_put_branch(out);
	else if (c && ft_strchr("uhwWs$c", c))
		prompt_dynamic(out, c);
	else
	{
		buf_addc(out, '\\');
		if (c)
			buf_addc(out, c);
	}
}

/*
** Builds the prompt from $PS1, or $PS2 for a continuation line.
*/
char	*prompt_build(int cont)
{
	t_buf	out;
	const char	*ps;

	ps = var_get("PS1");
	if (cont)
		ps = var_get("PS2");
	buf_init(&out);
	while (ps && *ps)
	{
		if (*ps == '\\')
		{
			prompt_escape(&out, ps[1]);
			ps += (ps[1] != '\0');
		}
		else
			buf_addc(&out, *ps);
		ps++;
	}
	return (buf_take(&out));
}
