#include "sh.h"

/*
** True if the current token is exactly this unquoted word.
*/
int	p_is_word(t_parser *p, const char *word)
{
	return (p->tok->type == T_WORD && str_eq(p->tok->str, word));
}

void	p_expect(t_parser *p, const char *word)
{
	if (p_is_word(p, word))
		p_next(p);
	else
		p_error(p);
}

int	is_reserved(const char *word)
{
	static const char	*words[] = {"if", "then", "else", "elif", "fi",
		"while", "until", "do", "done", "for", "in", "case", "esac", "{",
		"}", "!", NULL};
	int	i;

	i = 0;
	while (words[i] && !str_eq(words[i], word))
		i++;
	return (words[i] != NULL);
}

/*
** A list stops in front of anything that closes the enclosing construct.
*/
int	p_at_list_end(t_parser *p)
{
	static const char	*closers[] = {"then", "else", "elif", "fi", "do",
		"done", "esac", "}", NULL};
	int	i;

	if (p->tok->type == T_EOF || p->tok->type == T_RPAREN
		|| p->tok->type == T_DSEMI)
		return (1);
	if (p->tok->type != T_WORD)
		return (0);
	i = 0;
	while (closers[i] && !str_eq(closers[i], p->tok->str))
		i++;
	return (closers[i] != NULL);
}

/*
** A list must hold at least one command inside compound commands.
*/
t_node	*parse_body(t_parser *p)
{
	t_node	*body;

	p_skip_newlines(p);
	if (p->status == PARSE_OK && p_at_list_end(p))
	{
		p_error(p);
		return (NULL);
	}
	body = parse_list(p);
	return (body);
}
