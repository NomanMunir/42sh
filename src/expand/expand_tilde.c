#include "sh.h"

/*
** Directory named by a tilde prefix: ~ ~+ ~- ~user. NULL if unknown.
*/
static const char	*tilde_dir(const char *user)
{
	struct passwd	*pw;
	const char	*home;

	if (str_eq(user, "+"))
		return (var_get("PWD"));
	if (str_eq(user, "-"))
		return (var_get("OLDPWD"));
	if (*user)
		pw = getpwnam(user);
	else
	{
		home = var_get("HOME");
		if (home)
			return (home);
		pw = getpwuid(getuid());
	}
	if (!pw)
		return (NULL);
	return (pw->pw_dir);
}

/*
** The prefix runs up to the first '/' (or ':' in an assignment) and must
** be made of plain login-name characters.
*/
static size_t	tilde_prefix_len(t_exp *e, const char *s)
{
	size_t	len;

	len = 1;
	while (s[len] && s[len] != '/')
	{
		if (s[len] == ':' && (e->flags & EXP_ASSIGN))
			break ;
		if (!ft_isalnum(s[len]) && !ft_strchr("._+-", s[len]))
			return (0);
		len++;
	}
	return (len);
}

/*
** s points at '~'. Appends the home directory (never split or globbed),
** or the tilde itself when the prefix does not name anything.
*/
size_t	exp_tilde(t_exp *e, const char *s)
{
	size_t	len;
	char	*user;
	const char	*dir;

	len = tilde_prefix_len(e, s);
	dir = NULL;
	if (len > 0)
	{
		user = xstrndup(s + 1, len - 1);
		dir = tilde_dir(user);
		free(user);
	}
	if (!dir)
	{
		exp_put(e, '~', M_LIT);
		return (1);
	}
	exp_puts(e, dir, M_QUOTED);
	return (len);
}
