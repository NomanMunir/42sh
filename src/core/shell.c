#include "sh.h"

t_shell	*sh(void)
{
	static t_shell	shell;

	return (&shell);
}

static void	hashent_free(void *entry)
{
	free(((t_hashent *)entry)->path);
	free(entry);
}

/*
** State shared by every kind of session; the interactive parts (job
** control, line editing) are set up later, only when they are needed.
*/
void	sh_init(char **argv, char **envp)
{
	t_shell	*s;

	s = sh();
	map_init(&s->vars, var_free);
	map_init(&s->aliases, free);
	map_init(&s->funcs, NULL);
	map_init(&s->hash, hashent_free);
	strv_init(&s->comp);
	s->name = xstrdup(argv[0]);
	s->pos = xcalloc(1, sizeof(char *));
	s->pid = getpid();
	s->pgid = getpgrp();
	s->tty = -1;
	vars_import(envp);
	vars_defaults();
}

void	sh_destroy(void)
{
	t_shell	*s;

	s = sh();
	if (s->interactive)
		line_cleanup();
	jobs_forget();
	procsub_close();
	map_free(&s->vars);
	map_free(&s->aliases);
	map_free(&s->funcs);
	map_free(&s->hash);
	strv_free(&s->comp);
	free_strs(s->pos);
	free(s->name);
	s->pos = NULL;
	s->name = NULL;
	if (s->tty >= 0)
		close(s->tty);
	s->tty = -1;
}
