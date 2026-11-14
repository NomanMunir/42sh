# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: 42sh                                       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#                                                      #+#    #+#              #
#                                                     ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= 42sh

CC			= gcc
CFLAGS		= -Wall -Wextra -Werror
DEPFLAGS	= -MMD -MP

SRC_DIR		= src
INC_DIR		= include
OBJ_DIR		= obj
LIBFT_DIR	= libft

LIBFT		= $(LIBFT_DIR)/libft.a
INCLUDES	= -I$(INC_DIR) -I$(LIBFT_DIR)
LDFLAGS		= -L$(LIBFT_DIR)
LDLIBS		= -lft -lreadline -lncurses

CORE_SRC	= core/input.c core/main.c core/run.c core/run_source.c \
			  core/shell.c core/signals.c

UTILS_SRC	= utils/buf.c utils/buf_take.c utils/map.c utils/map_iter.c \
			  utils/strs.c utils/strv.c utils/text.c utils/xalloc.c \
			  utils/xstring.c

LEXER_SRC	= lexer/lex_heredoc.c lexer/lex_word.c lexer/lexer.c \
			  lexer/scan.c lexer/scan_group.c lexer/token.c

PARSER_SRC	= parser/alias_expand.c parser/node.c parser/node_dup.c \
			  parser/parse_case.c parser/parse_command.c \
			  parser/parse_flow.c parser/parse_list.c \
			  parser/parse_simple.c parser/parse_words.c parser/parser.c

EXPAND_SRC	= expand/arith.c expand/arith_binary.c expand/arith_unary.c \
			  expand/arith_var.c expand/expand.c expand/expand_brace.c \
			  expand/expand_cmdsub.c expand/expand_cond.c \
			  expand/expand_dollar.c expand/expand_fields.c \
			  expand/expand_procsub.c expand/expand_quote.c \
			  expand/expand_run.c expand/expand_tilde.c \
			  expand/expand_trim.c expand/glob.c expand/pattern.c

VARS_SRC	= vars/param.c vars/var.c vars/var_defaults.c vars/var_env.c

EXEC_SRC	= exec/exec.c exec/exec_assign.c exec/exec_cmd.c \
			  exec/exec_external.c exec/exec_flow.c exec/exec_func.c \
			  exec/exec_job.c exec/exec_logic.c exec/exec_path.c \
			  exec/exec_pipe.c exec/heredoc.c exec/redir.c \
			  exec/redir_open.c exec/redir_save.c

JOBS_SRC	= jobs/job_control.c jobs/job_find.c jobs/job_fork.c \
			  jobs/job_print.c jobs/job_status.c jobs/job_table.c \
			  jobs/job_wait.c

BUILTINS_SRC	= builtins/alias.c builtins/builtins.c builtins/cd.c \
			  builtins/cd_path.c builtins/echo.c builtins/exec.c \
			  builtins/exit.c builtins/export.c builtins/fc.c \
			  builtins/fc_range.c builtins/fc_run.c builtins/fg_bg.c \
			  builtins/flow.c builtins/hash.c builtins/history.c \
			  builtins/jobs.c builtins/kill.c builtins/kill_list.c \
			  builtins/kill_sig.c builtins/misc.c builtins/pwd.c \
			  builtins/quote.c builtins/read.c builtins/read_split.c \
			  builtins/set.c builtins/set_opt.c builtins/source.c \
			  builtins/test.c builtins/test_ops.c builtins/type.c \
			  builtins/unset.c builtins/wait.c

LINE_SRC	= line/comp_collect.c line/comp_gen.c line/comp_quote.c \
			  line/complete.c line/edit.c line/hist_access.c \
			  line/hist_expand.c line/history.c line/keys.c line/line.c \
			  line/prompt.c line/prompt_git.c

SRC_FILES	= $(CORE_SRC) \
			  $(UTILS_SRC) \
			  $(LEXER_SRC) \
			  $(PARSER_SRC) \
			  $(EXPAND_SRC) \
			  $(VARS_SRC) \
			  $(EXEC_SRC) \
			  $(JOBS_SRC) \
			  $(BUILTINS_SRC) \
			  $(LINE_SRC)

SRCS		= $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS		= $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

# The libft Makefile decides by itself whether anything must be rebuilt;
# 42sh is only linked again when libft.a really changed.
$(LIBFT): FORCE
	@$(MAKE) --no-print-directory -q -C $(LIBFT_DIR) \
		|| $(MAKE) --no-print-directory -C $(LIBFT_DIR)

FORCE:

clean:
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) clean
	rm -rf $(OBJ_DIR)

fclean: clean
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

debug: CFLAGS += -g3
debug: re

run: all
	./$(NAME)

test: all
	@bash tests/run_tests.sh

leaks: all
	valgrind --leak-check=full --show-leak-kinds=definite,indirect \
		--errors-for-leak-kinds=definite,indirect ./$(NAME)

help:
	@echo "make          build $(NAME) (and libft)"
	@echo "make clean    remove the object files"
	@echo "make fclean   remove the object files, libft.a and $(NAME)"
	@echo "make re       rebuild everything"
	@echo "make debug    rebuild everything with debug symbols (-g3)"
	@echo "make run      build, then start an interactive $(NAME)"
	@echo "make test     build, then run the test suite (tests/)"
	@echo "make leaks    build, then start $(NAME) under valgrind"
	@echo "make help     show this help"

-include $(DEPS)

.PHONY: all clean fclean re debug run test leaks help FORCE
