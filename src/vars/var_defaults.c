#include "sh.h"

/*
** PWD is trusted only if it really names the current directory.
*/
static void	default_pwd(void)
{
	struct stat	here;
	struct stat	named;
	char	*pwd;
	char	*cwd;

	pwd = var_get("PWD");
	if (pwd && pwd[0] == '/' && stat(pwd, &named) == 0
		&& stat(".", &here) == 0 && named.st_ino == here.st_ino
		&& named.st_dev == here.st_dev)
	{
		var_set("PWD", NULL, V_EXPORT);
		return ;
	}
	cwd = getcwd(NULL, 0);
	if (cwd)
		var_set("PWD", cwd, V_EXPORT);
	free(cwd);
}

static void	default_shlvl(void)
{
	char	*value;
	char	*text;
	long	level;

	level = 0;
	value = var_get("SHLVL");
	if (!value || !parse_long(value, &level) || level < 0 || level > 998)
		level = 0;
	text = xitoa(level + 1);
	var_set("SHLVL", text, V_EXPORT);
	free(text);
}

static void	default_prompt(void)
{
	char	*term;

	term = var_get("TERM");
	if (!term || str_eq(term, "dumb"))
		var_set("PS1", "42sh:\\w\\$ ", 0);
	else
		var_set("PS1", "\\[\\e[1;36m\\]42sh\\[\\e[0m\\] "
			"\\[\\e[1;34m\\]\\w\\[\\e[0;33m\\]\\g\\[\\e[0m\\] "
			"\\c\\$\\[\\e[0m\\] ", 0);
	var_set("PS2", "> ", 0);
}

/*
** Variables every session starts with.
*/
void	vars_defaults(void)
{
	char	*home;
	char	*histfile;

	default_pwd();
	default_shlvl();
	default_prompt();
	home = var_get("HOME");
	if (home && !var_get("HISTFILE"))
	{
		histfile = xjoin(home, "/.42sh_history");
		var_set("HISTFILE", histfile, 0);
		free(histfile);
	}
	if (!var_get("HISTSIZE"))
		var_set("HISTSIZE", "1000", 0);
	var_set("IFS", " \t\n", 0);
	var_set("OPTIND", "1", 0);
}
