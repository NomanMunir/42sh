#include "sh.h"

/*
** The only global of the shell: the number of the last signal caught,
** written by the handler and read by the main flow.
*/
static volatile sig_atomic_t	g_signal;

static void	on_signal(int sig)
{
	g_signal = sig;
}

int	sig_peek(void)
{
	return (g_signal);
}

int	sig_take(void)
{
	int	sig;

	sig = g_signal;
	g_signal = 0;
	return (sig);
}

/*
** An interactive shell survives the keyboard signals: ^C only interrupts
** the command being typed or run (the handler is installed without
** SA_RESTART so that blocking calls return), the others are ignored.
*/
void	signals_interactive(void)
{
	struct sigaction	act;

	ft_bzero(&act, sizeof(act));
	sigemptyset(&act.sa_mask);
	act.sa_handler = on_signal;
	sigaction(SIGINT, &act, NULL);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
}

/*
** Children get the default actions back. The terminal stop signals are
** only restored for real jobs (see sh_enter_child).
*/
void	signals_default(int job)
{
	g_signal = 0;
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	if (!job)
		return ;
	signal(SIGTSTP, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
}
