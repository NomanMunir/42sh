#include "sh.h"

/*
** Each token remembers the aliases it came from (":ls:ll:") so that an
** alias is never expanded again inside its own expansion.
*/
static int	chain_has(const char *chain, const char *name)
{
	char	*key;
	int	found;

	if (!chain)
		return (0);
	key = xjoin3(":", name, ":");
	found = (ft_strnstr(chain, key, ft_strlen(chain)) != NULL);
	free(key);
	return (found);
}

/*
** dst takes over the content of src; the src cell itself is released.
*/
static void	tok_move(t_token *dst, t_token *src)
{
	free(dst->str);
	free(dst->heredoc);
	free(dst->chain);
	*dst = *src;
	free(src);
}

/*
** Tags the tokens of the expansion and returns the last real one (the
** trailing newline / end-of-input tokens are not part of the alias).
*/
static t_token	*alias_tag(t_token *sub, const char *chain, t_token **rest)
{
	t_token	*last;

	last = NULL;
	while (sub->type != T_EOF
		&& !(sub->type == T_NEWLINE && sub->next->type == T_EOF))
	{
		free(sub->chain);
		sub->chain = xstrdup(chain);
		last = sub;
		sub = sub->next;
	}
	*rest = sub;
	return (last);
}

/*
** Replaces tok by the token list sub. An alias ending with a blank lets
** the following word be alias-expanded too.
*/
static void	alias_splice(t_token *tok, t_token *sub, const char *chain,
	int blank)
{
	t_token	*after;
	t_token	*last;
	t_token	*rest;

	after = tok->next;
	last = alias_tag(sub, chain, &rest);
	tok_free_list(rest);
	if (blank && after->type == T_WORD)
		after->flags |= TF_ALIASABLE;
	if (!last)
	{
		tok_move(tok, after);
		return ;
	}
	last->next = after;
	tok_move(tok, sub);
}

/*
** If tok is an alias name, substitutes its value in the token list and
** returns 1 (tok then holds the first token of the expansion).
*/
int	alias_expand_at(t_token *tok)
{
	const char	*value;
	char	*chain;
	t_token	*sub;
	int	status;

	if (tok->type != T_WORD)
		return (0);
	value = map_get(&sh()->aliases, tok->str);
	if (!value || chain_has(tok->chain, tok->str))
		return (0);
	sub = lex(value, 1, &status);
	if (status != PARSE_OK)
	{
		tok_free_list(sub);
		return (0);
	}
	if (tok->chain)
		chain = xjoin3(tok->chain, tok->str, ":");
	else
		chain = xjoin3(":", tok->str, ":");
	status = (*value && ft_strchr(" \t", value[ft_strlen(value) - 1]));
	alias_splice(tok, sub, chain, status);
	free(chain);
	return (1);
}
