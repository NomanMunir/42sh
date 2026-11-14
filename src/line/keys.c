#include "sh.h"

/*
** Opens the line being typed in an editor and runs what comes back
** ("v" in vi command mode, C-x C-e in readline mode). The status of the
** last command is not affected by the editor.
*/
static int	key_edit_line(int count, int key)
{
	char	*edited;
	int	status;

	(void)count;
	status = sh()->status;
	rl_deprep_terminal();
	edited = edit_text(rl_line_buffer, NULL);
	rl_prep_terminal(0);
	sh()->status = status;
	if (sh()->unwind != U_EXIT)
		sh()->unwind = U_NONE;
	write(STDERR_FILENO, "\n", 1);
	rl_on_new_line();
	if (!edited)
	{
		rl_redisplay();
		return (0);
	}
	rl_replace_line(edited, 0);
	rl_point = rl_end;
	free(edited);
	rl_redisplay();
	return (rl_newline(1, key));
}

/*
** Key bindings added on top of the defaults of each editing mode.
*/
void	line_bind_keys(void)
{
	rl_bind_key_in_map('v', key_edit_line, vi_movement_keymap);
	rl_bind_key_in_map('\t', rl_complete, vi_insertion_keymap);
	rl_bind_keyseq_in_map("\\C-x\\C-e", key_edit_line,
		emacs_standard_keymap);
}
