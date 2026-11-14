#ifndef UTILS_H
# define UTILS_H

# include <stddef.h>
# include <sys/types.h>

/*
** t_buf: growable string. t_strv: growable NULL-terminated string array.
** t_map: chained hash table (string -> pointer) used for variables,
** aliases, functions and the command hash table.
*/
typedef struct s_buf
{
	char	*s;
	size_t	len;
	size_t	cap;
}	t_buf;

typedef struct s_strv
{
	char	**v;
	int	len;
	int	cap;
}	t_strv;

typedef struct s_ment
{
	char	*key;
	void	*val;
	struct s_ment	*next;
}	t_ment;

typedef void	(*t_delfn)(void *);

typedef struct s_map
{
	t_ment	**tab;
	size_t	size;
	size_t	count;
	t_delfn	del;
}	t_map;

void	*xmalloc(size_t size);
void	*xcalloc(size_t count, size_t size);
void	*xrealloc(void *ptr, size_t size);
char	*xstrdup(const char *s);
char	*xstrndup(const char *s, size_t n);
char	*xjoin(const char *a, const char *b);
char	*xjoin3(const char *a, const char *b, const char *c);
char	*xitoa(long n);
int	str_eq(const char *a, const char *b);
int	is_name(const char *s);
size_t	name_len(const char *s);
int	parse_long(const char *s, long *out);
int	status_code(int wstatus);
void	buf_init(t_buf *b);
void	buf_addc(t_buf *b, char c);
void	buf_addn(t_buf *b, const char *s, size_t n);
void	buf_adds(t_buf *b, const char *s);
void	buf_addnum(t_buf *b, long n);
char	*buf_take(t_buf *b);
void	buf_free(t_buf *b);
int	buf_write(t_buf *b, int fd, const char *who);
void	strv_init(t_strv *sv);
void	strv_push(t_strv *sv, char *s);
char	**strv_take(t_strv *sv);
void	strv_free(t_strv *sv);
void	free_strs(char **v);
int	strs_len(char **v);
void	strs_sort(char **v, int n);
char	**strs_dup(char **v);
char	*strs_join(char **v, const char *sep);
size_t	map_hash(const char *key);
void	map_init(t_map *m, t_delfn del);
void	*map_get(t_map *m, const char *key);
void	map_set(t_map *m, const char *key, void *val);
int	map_del(t_map *m, const char *key);
void	map_grow(t_map *m);
void	map_clear(t_map *m);
void	map_free(t_map *m);
char	**map_keys(t_map *m);

#endif
