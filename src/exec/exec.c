#include "sh.h"

static int	exec_dispatch(t_node *n, int flags)
{
	static const t_execfn	table[] = {exec_cmd, exec_pipe, exec_and, exec_or,
		exec_seq, exec_bg, exec_not, exec_subshell, exec_group, exec_if,
		exec_loop, exec_loop, exec_for, exec_case, NULL, exec_funcdef};

	return (table[n->type](n, flags));
}

/*
** Compound commands with redirections: the redirections surround the
** whole command and are undone afterwards.
*/
static int	exec_redirected(t_node *n, int flags)
{
	t_fdsave	*saved;
	int	status;

	saved = NULL;
	status = 1;
	if (redir_apply(n->redirs, &saved) == 0)
		status = exec_dispatch(n, flags);
	redir_restore(&saved);
	procsub_close();
	return (status);
}

/*
** Runs a tree and returns its exit status, which also becomes $?.
** While the shell is unwinding (exit, return, interrupt) the status set by
** whoever started the unwinding is preserved.
*/
int	exec_node(t_node *n, int flags)
{
	t_shell	*s;
	int	status;

	s = sh();
	if (s->interactive && sig_peek() == SIGINT && !s->unwind)
	{
		s->status = 130;
		s->unwind = U_INT;
	}
	if (s->unwind)
		return (s->status);
	if (!n)
		status = 0;
	else if (n->redirs && n->type != N_CMD && n->type != N_SUBSHELL)
		status = exec_redirected(n, flags & ~X_EXIT);
	else
		status = exec_dispatch(n, flags);
	if (s->unwind < U_RETURN)
		s->status = status;
	return (s->status);
}

/*
** Consumes one level of a pending break or continue. Returns 1 when the
** loop that called it must stop.
*/
int	loop_control(void)
{
	t_shell	*s;

	s = sh();
	if (s->unwind == U_CONTINUE)
	{
		if (--s->unwind_n > 0)
			return (1);
		s->unwind = U_NONE;
		return (0);
	}
	if (s->unwind == U_BREAK && --s->unwind_n <= 0)
		s->unwind = U_NONE;
	else if (s->unwind == U_NONE)
		return (0);
	return (1);
}
