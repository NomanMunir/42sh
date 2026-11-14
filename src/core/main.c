#include "sh.h"

/*
** 42sh -c string [name [argument...]]
*/
static void	run_option_c(char **args)
{
	if (!args[0])
	{
		dprintf(STDERR_FILENO, "42sh: -c: option requires an argument\n");
		sh()->status = 2;
		return ;
	}
	if (args[1])
	{
		free(sh()->name);
		sh()->name = xstrdup(args[1]);
		free_strs(params_swap(strs_dup(args + 2)));
	}
	sh_run_string(args[0]);
}

/*
** 42sh script [argument...]
*/
static void	run_script(char **args)
{
	free(sh()->name);
	sh()->name = xstrdup(args[0]);
	free_strs(params_swap(strs_dup(args + 1)));
	sh()->status = sh_run_file(args[0], 1);
}

/*
** The startup file of interactive sessions, if the user has one.
*/
static void	run_startup_file(void)
{
	char	*home;
	char	*path;

	home = var_get("HOME");
	if (!home)
		return ;
	path = xjoin(home, "/.42shrc");
	if (access(path, R_OK) == 0)
		sh_run_file(path, 0);
	free(path);
	if (sh()->unwind != U_EXIT)
		sh()->unwind = U_NONE;
}

/*
** Commands come from the standard input. The session is interactive
** when both the input and the error output are a terminal.
*/
static void	run_stdin(void)
{
	t_input	in;

	ft_bzero(&in, sizeof(in));
	in.kind = IN_FD;
	in.fd = STDIN_FILENO;
	if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
	{
		in.kind = IN_TTY;
		sh()->interactive = 1;
		jobctl_init();
		line_init();
		run_startup_file();
	}
	sh_loop(&in);
}

int	main(int argc, char **argv, char **envp)
{
	int	status;

	(void)argc;
	sh_init(argv, envp);
	if (argv[1] && str_eq(argv[1], "--"))
		argv++;
	if (argv[1] && str_eq(argv[1], "-c"))
		run_option_c(argv + 2);
	else if (argv[1] && argv[1][0] == '-' && argv[1][1])
	{
		dprintf(STDERR_FILENO, "42sh: %s: invalid option\n", argv[1]);
		sh()->status = 2;
	}
	else if (argv[1])
		run_script(argv + 1);
	else
		run_stdin();
	status = sh()->status;
	sh_destroy();
	return (status);
}
