#ifndef LINE_H
# define LINE_H

# include "utils.h"

# define C_FILE 0
# define C_CMD 1
# define C_VAR 2
# define C_VARBRACE 3
# define C_VARNAME 4
# define C_DIR 5
# define C_ALIAS 6
# define C_SETOPT 7
# define C_JOB 8

# define HIST_DEFAULT_SIZE 1000

void	line_init(void);
void	line_cleanup(void);
char	*line_read(int cont);
void	line_set_mode(int vi);
void	line_bind_keys(void);
char	*prompt_build(int cont);
void	prompt_escape(t_buf *out, char c);
void	prompt_put_safe(t_buf *out, const char *s);
char	*prompt_git_branch(void);
void	hist_load(void);
void	hist_save(void);
void	hist_add(const char *text);
char	*hist_expand(const char *line, int *changed);
const char	*hist_line(int pos);
void	hist_drop_last(void);
char	*edit_text(const char *text, const char *editor);
char	**line_complete(const char *text, int start, int end);
int	comp_context(int start);
void	comp_collect(int ctx, const char *text);
char	*comp_gen_list(const char *text, int state);
char	*comp_gen_dir(const char *text, int state);
void	comp_quote_init(void);

#endif
