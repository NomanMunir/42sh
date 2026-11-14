#include "sh.h"

/*
** Calls a shell function: its arguments become the positional parameters
** for the duration of the call. The body runs on a private copy so that
** the function may redefine itself.
*/
int	func_call(char **argv)
{
	t_shell	*s;
	t_node	*body;
	char	**saved;
	int	loops;

	s = sh();
	if (s->nest >= NEST_MAX)
	{
		dprintf(STDERR_FILENO, "42sh: %s: maximum nesting level exceeded\n",
			argv[0]);
		return (1);
	}
	body = node_dup(map_get(&s->funcs, argv[0]));
	saved = params_swap(strs_dup(argv + 1));
	loops = s->loop_depth;
	s->loop_depth = 0;
	s->nest++;
	s->func_depth++;
	exec_node(body, 0);
	if (s->unwind == U_RETURN)
		s->unwind = U_NONE;
	s->func_depth--;
	s->nest--;
	s->loop_depth = loops;
	free_strs(params_swap(saved));
	node_free(body);
	return (s->status);
}
