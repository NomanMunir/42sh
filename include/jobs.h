#ifndef JOBS_H
# define JOBS_H

# include <termios.h>
# include "ast.h"

# define JS_RUNNING 0
# define JS_STOPPED 1
# define JS_DONE 2

# define JP_NORMAL 0
# define JP_LONG 1
# define JP_PGID 2

typedef struct s_proc
{
	pid_t	pid;
	int	status;
	int	state;
	struct s_proc	*next;
}	t_proc;

/*
** A job is one pipeline (or one backgrounded list) sharing a process group.
** seq orders jobs by last use so that %+ and %- can be computed.
*/
typedef struct s_job
{
	int	id;
	pid_t	pgid;
	char	*text;
	t_proc	*procs;
	int	bg;
	int	notified;
	long	seq;
	int	has_tmodes;
	struct termios	tmodes;
	struct s_job	*next;
}	t_job;

void	jobctl_init(void);
t_job	*job_new(const char *text);
void	job_remove(t_job *job);
void	jobs_forget(void);
int	jobs_stopped(void);
pid_t	job_fork(t_job *job, int bg);
void	sh_enter_child(int job);
int	job_wait_fg(t_job *job);
void	job_wait(t_job *job);
void	jobs_reap(void);
void	jobs_update(pid_t pid, int wstatus);
int	job_state(t_job *job);
int	job_status(t_job *job);
pid_t	job_last_pid(t_job *job);
t_job	*job_current(t_job *exclude);
t_job	*job_find(const char *spec);
void	job_print(t_job *job, int mode, int fd);
void	jobs_notify(void);
void	job_background(t_job *job);
int	job_continue(t_job *job, int fg);
t_job	*job_launch_single(t_node *n, int bg);
t_job	*job_launch_pipeline(t_node *n, int bg);

#endif
