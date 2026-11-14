#include "sh.h"

/*
** $PWD can be shown as the logical directory only if it is an absolute
** path without "." or ".." that really names the current directory.
*/
static int	pwd_is_valid(const char *pwd)
{
	struct stat	named;
	struct stat	here;
	char	*canon;
	int	same;

	if (!pwd || pwd[0] != '/')
		return (0);
	canon = path_canon(pwd);
	same = str_eq(canon, pwd);
	free(canon);
	if (!same || stat(pwd, &named) < 0 || stat(".", &here) < 0)
		return (0);
	return (named.st_ino == here.st_ino && named.st_dev == here.st_dev);
}

/*
** pwd [-L | -P]
*/
int	bi_pwd(char **argv)
{
	t_buf	out;
	char	*cwd;
	int	flags;

	if (opt_parse(argv, "LP", &flags) < 0)
		return (2);
	cwd = NULL;
	if (!((flags & 2) && !(flags & 1)) && pwd_is_valid(var_get("PWD")))
		cwd = xstrdup(var_get("PWD"));
	if (!cwd)
		cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		dprintf(STDERR_FILENO, "42sh: pwd: %s\n", strerror(errno));
		return (1);
	}
	buf_init(&out);
	buf_adds(&out, cwd);
	buf_addc(&out, '\n');
	free(cwd);
	return (buf_write(&out, STDOUT_FILENO, "pwd"));
}
